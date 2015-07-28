#!/bin/sh

/sbin/ifconfig eth0 192.168.1.128 netmask 255.255.255.0

ifconfig eth0 up

route add -net 192.168.1.0 netmask 255.255.255.0 gw 192.168.1.1 dev eth0

sleep 2

ifconfig eth0 down
ifconfig eth0 up
