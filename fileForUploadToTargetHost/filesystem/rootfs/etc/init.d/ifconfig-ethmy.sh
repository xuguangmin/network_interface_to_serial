#!/bin/sh

CONFIG_FILE=/etc/ntos/config.ini

/sbin/ifconfig lo 127.0.0.1
/sbin/ifconfig eth0 192.168.1.100 netmask 255.255.255.0

/sbin/ifconfig eth0 down
/sbin/ifconfig eth0 hw ether 00E04AA80164
/sbin/ifconfig eth0 up



subnetmask=$(cat $CONFIG_FILE |grep "subnetmask =" |cut -d' ' -f3 |awk '{print $1}')
#echo $subnetmask

ipstr=$(cat $CONFIG_FILE |grep "ip =" |cut -d' ' -f3 |awk '{print $1}')
#echo $ipstr

dev=$(echo $(ifconfig |grep "eth" |cut -d' ' -f1))
#echo $dev

gateway=$(cat $CONFIG_FILE |grep "gateway =" |cut -d' ' -f3 |awk '{print $1}')
#echo $gateway

echo $dev $ipstr netmask $subnetmask

#ip last bit to hex	for mac last bit
ipHex=$(echo `printf "%x" $(ifconfig |grep "inet addr:" |grep -v "127.0.0.1" |cut -d: -f2 |cut -d. -f4 |awk '{print $1}')`)

#-----------------------------------------------------------------------------------------------


#echo "start ifconfig"
ifconfig $dev $ipstr netmask $subnetmask
#echo "end ifconfig"

#echo "start gateway"
route add -net 192.168.0.0 netmask $subnetmask gw $gateway dev $dev
#echo "end gateway"

#echo "setup mac as 00E04AA801"$ipHex
ifconfig $dev down
ifconfig $dev hw ether "00E04AA802"$ipHex
ifconfig $dev up

ifconfig



