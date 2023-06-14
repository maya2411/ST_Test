#!/bin/sh
# I2C 0
echo 1 > /sys/class/gpio/gpio52/value
echo 1 > /sys/class/gpio/gpio51/value
# I2C 2
echo 1 > /sys/class/gpio/gpio133/value
echo 1 > /sys/class/gpio/gpio131/value
