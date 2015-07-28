mkimage -n 'linux-2.6.29' -A arm -O linux -T kernel -C none -a 0xc0008000 -e 0xc0008000 -d zImage uImage

#!/bin/sh

/sbin/ifconfig eth0 192.168.1.128 netmask 255.255.255.0
/sbin/ifconfig eth1 192.168.0.129 netmask 255.255.255.0

ifconfig eth1 up
ifconfig eth0 up

route add -net 192.168.1.0 netmask 255.255.255.0 gw 192.168.1.1 dev eth0
route add -net 192.168.0.0 netmask 255.255.255.0 gw 192.168.0.3 dev eth1

sleep 2

ifconfig eth1 down
ifconfig eth1 up

sleep 2

ifconfig eth0 down
ifconfig eth0 up

