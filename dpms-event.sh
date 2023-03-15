#!/bin/sh
#
# This script is called by handle_dpms

case $1 in
    off)
        logger -t "dpms-event" "Disabling keyboard LEDs"
        /usr/local/sbin/cherryrgb_cli -b off animation static fast ff90ff
        ;;
    on)
        logger -t "dpms-event" "Enabling keyboard LEDs"
        /usr/local/sbin/cherryrgb_cli -b full animation static fast ff90ff
        ;;
esac
