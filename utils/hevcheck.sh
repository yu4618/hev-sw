#!/bin/sh
# called from the udev rule at /etc/udev/rules.d/88-hev.rules when arduino connected

echo "Arduino connected at $(date)" >> /tmp/hevcheck.log
