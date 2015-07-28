#!/bin/sh

echo "if you wanna enter console, please input 'q':"
read -t 3 cmd
if [ "$cmd" = "q" ];then
exit 0
fi

insmod /usr/lib/FPGA_Jikong_ir.ko
insmod /usr/lib/FPGA_Jikong_relay.ko
insmod /usr/lib/FPGA_Jikong_uart.ko
insmod /usr/lib/Jikong_Led.ko

rm -rf /mnt/jikong/tmp_ccc

cd /mnt/jikong
./ccc_upgrade
./ccc_server
./ccc_console
