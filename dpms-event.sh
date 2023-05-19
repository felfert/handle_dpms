#!/bin/sh
#
# This script is called by handle_dpms

case $1 in
    off)
        logger -t "dpms-event" "Disabling keyboard LEDs"
        cherryrgb_ncli -b off animation static fast ff90ff
        ;;
    on)
        logger -t "dpms-event" "Enabling keyboard LEDs"
        cherryrgb_ncli -b full animation static fast ff90ff
        ;;
esac
