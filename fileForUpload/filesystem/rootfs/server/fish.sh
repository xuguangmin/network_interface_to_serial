#!/bin/sh

echo "running fish.sh"
echo ""

file="/upgrade/upgrade.tar.gz"
upsh="/upgrade/online/up.sh"
dire="/upgrade"
boa="/server/boa"
ntos="/server/ntos_server"

echo "if you wanna enter console, please input 'q':"
read -t 3 cmd
if [ "$cmd" = "q" ];then
	exit 0
fi

echo  "checking upgrade"
if [ -f "$file" ]; then
	echo "$file unrar"
	tar xvf $file -C /upgrade/
	mv /upgrade/online/*.sh /upgrade/online/up.sh

	if [ -f "$upsh" ]; then
		if [ ! -x "$upsh" ]; then
			echo "error: upgrade script peration not permitted"
			echo "modify: chmod 777 $upsh      ok"
			chmod 777 $upsh
			echo "updating ..."
			$upsh
			echo "updating ok"
		else
			echo "updating ..."
			$upsh
			echo "updating ok"
		fi
	else
		echo "error: not found script for running in updata paceage, skip update"
	fi

	echo "if you wanna enter console, please input 'q':"
	read -t 2 cmd
	if [ "$cmd" = "q" ];then
		exit 0
	fi

	echo "removing update package /upgrade/*"
	rm -rf /upgrade/*
else 
	echo "not found file for updating, skip update"
fi






cd /server

if [ ! -d "/server" ]; then
	mkdir /server
fi
if [ ! -d "/server/www" ]; then
	mkdir /server/www
fi
if [ ! -d "/server/www/cgi-bin" ]; then
	mkdir /server/www/cgi-bin
fi
if [ ! -d "$dire" ]; then
	mkdir /upgrade
fi


if [ -f "/server/ifconfig-eth.sh" ]; then
	cp /server/ifconfig-eth.sh /etc/init.d/
	reboot;
fi


echo  "insmod reset key driver"
insmod /server/Jikong_uart_convert_reset.ko
if [ ! -e "/server/test_uart_convert_board" ]; then
	echo "Error: Can't find test_uart_convert_board"
elif [ ! -x "/server/test_uart_convert_board" ]; then
	echo "add permission test_uart_convert_board"
	chmod +x /server/test_uart_convert_board 
else
	echo "running test_uart_convert_board"
	/server/test_uart_convert_board 
fi

chmod +x /server/www/cgi-bin/*
echo "starting boa service"
	if [ ! -d "var/log" ]; then
		mkdir /var/log
	fi
	if [ ! -d "var/log/boa" ]; then
		mkdir /var/log/boa
	fi
	if [ ! -f "var/log/boa/error_log" ]; then
		touch /var/log/boa/error_log
	fi
	if [ ! -f "var/log/boa/access_log" ]; then
		touch /var/log/boa/access_log
	fi
	if [ ! -d "/etc/boa" ]; then
		mkdir /etc/boa
	fi
	if [ ! -f "/etc/boa/boa.conf" ]; then
		cp /server/boa.conf /etc/boa
	fi

	#判断mime.type文件是否存在,该文件定义了boa解析的数据类型
	if [ ! -f "/etc/mime.types" ]; then
		echo "cp /server/mime.types /etc/mime.types"
		cp /server/mime.types /etc/mime.types
	fi 
	


	if [ -f "$boa" ]; then
		if [ ! -x "$boa" ]; then
			echo ""
			echo "error: boa program peration not permitted"
			echo "modify: chmod 777 $boa"
			chmod 777 $boa
			echo "starting $boa"
			$boa
		else
			echo ""
			$boa
		fi
	else
		echo "boa not found"
	fi

echo "starting $ntos"
	if [ ! -d "/etc/ntos" ]; then
		mkdir /etc/ntos
	fi
	if [ ! -f "/etc/ntos/config.ini" ]; then
		cp /server/config.ini /etc/ntos/config.ini
	fi

	if [ -f "$ntos" ]; then
		if [ ! -x "$ntos" ]; then
			echo ""
			echo "error: boa program peration not permitted"
			chmod 777 $ntos echo "starting $ntos" $ntos else echo ""
			echo "starting $ntos"
			$ntos
		else
			$ntos
		fi
	else
		echo "error: ntos_server not found"
	fi



