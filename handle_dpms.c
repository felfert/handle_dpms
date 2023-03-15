/*
 * Listen to acpid. */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <time.h>

#ifndef ____GLIBC__
#ifndef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(expression) \
  (__extension__                                                             \
    ({ long int __result;                                                    \
       do __result = (long int) (expression);                                \
       while (__result == -1L && errno == EINTR);                            \
       __result; }))
#endif
#endif /* __GLIBC__ */

static const char *DPMS_EVENT_CMD = "/usr/libexec/dpms-event.sh";
static const char *ACPI_SOCK = "/var/run/acpid.socket";

#define MAX_BUFLEN	1024
static char * read_line(int fd) {
	static char *buf;
	int buflen = 64;
	int i = 0;
	int r;
	int searching = 1;

	while (searching) {
		buf = realloc(buf, buflen);
		if (!buf) {
			fprintf(stderr, "ERR: malloc(%d): %s\n",
				buflen, strerror(errno));
			return NULL;
		}
		memset(buf+i, 0, buflen-i);

		while (i < buflen) {
			r = TEMP_FAILURE_RETRY (read(fd, buf+i, 1) );
			if (r < 0) {
				/* we should do something with the data */
				fprintf(stderr, "ERR: read(): %s\n",
					strerror(errno));
				return NULL;
			} else if (r == 0) {
				/* signal this in an almost standard way */
				errno = EPIPE;
				return NULL;
			} else if (r == 1) {
				/* scan for a newline */
				if (buf[i] == '\n') {
					searching = 0;
					buf[i] = '\0';
					break;
				}
				i++;
			}
		}
		if (buflen >= MAX_BUFLEN) {
			break;
		}
		buflen *= 2;
	}

	return buf;
}

static int last_run_arg = -1;
static time_t last_run_time = -1;

static void run_event_handler(int on) {
    char cmd[1024];
    if (sizeof(cmd) <= snprintf(cmd, sizeof(cmd), "%s %s", DPMS_EVENT_CMD, on ? "on" : "off")) {
        fprintf(stderr, "cmd buffer overflow. NOT executing event-funtion\n");
        return;
    }
    time_t now = time(NULL);
    if ((last_run_arg != on) || (last_run_time != now)) {
        last_run_arg = on;
        last_run_time = now;
        system(cmd);
    }
}

int main(int argc, char **argv) {

    struct sockaddr_un addr;

    int fd = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0);
    if (fd < 0) {
        perror("socket");
        return 1;
    }
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    sprintf(addr.sun_path, "%s", ACPI_SOCK);
    int r = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (r < 0) {
        perror(ACPI_SOCK);
        close(fd);
        return 1;
    }
    while (1) {
        char *event;
        event = read_line(fd);
        if (event) {
            if (strstr(event, "VIDEOOUT unplug")) {
                run_event_handler(0);
                continue;
            }
            if (strstr(event, "VIDEOOUT plug")) {
                run_event_handler(1);
                continue;
            }
        } else if (errno == EPIPE) {
            fprintf(stderr, "connection to acpid closed\n");
            return 1;
        }
    }
    return 0;
}
