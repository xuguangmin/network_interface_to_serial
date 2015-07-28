// stdafx.cpp : 只包括标准包含文件的源文件
// eth_win_static.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用
// eth_win.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"


/*-------------------------------------------------------------------------*/
/**
   @file    eth.c
   @author  g.m 

   create on: 2014年 7月 21日 
*/
/*--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                                Includes
 ---------------------------------------------------------------------------*/

#ifndef __INIT_SOCK_H__
#define __INIT_SOCK_H__ 

#include <winsock2.h>

#pragma comment(lib, "WS2_32")
class CInitSock
{
public:
	CInitSock(BYTE minorVer = 2, BYTE majorVer = 2)
	{
		WSADATA wsaData;
		WORD sockVersion = MAKEWORD(minorVer, majorVer);
		//VERIFY(::WSAStartup(sockVersion, &wsaData)==0);
		WSAStartup(sockVersion, &wsaData);
	}

	~CInitSock()
	{
		::WSACleanup();
	}
};
#endif // __INIT_SOCK_H__

/*---------------------------------------------------------------------------
                            Function codes
 ---------------------------------------------------------------------------*/
int create_socket_stream_r()
{
	int sd;
	int val;

	sd = socket(AF_INET, SOCK_STREAM , IPPROTO_IP);
	if(sd < 0)
	{
		perror("socket:\r\n");	
		exit(-1);
	}

	val = 1;
	/*
	ret = setsockopt(sd,  SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
	if(ret != 0)
	{
		perror("setsockopt:\r\n");	
		exit(-1);
	}*/

	return sd;
}
int open_socket()
{
	return create_socket_stream_r();
}

// 初始化
void init_sockaddr_in(int sa, char* ipstr, int portnum, struct sockaddr_in *addr)
{
	addr->sin_family = sa; 
	addr->sin_port = htons(portnum);   /* port in network byte order */
	addr->sin_addr.S_un.S_addr = inet_addr(ipstr);
}


int connect_plx(char *ipstr, int pot)
{
	int sockfd;
	int ret;
	struct sockaddr_in raddr;
	
	if((sockfd = open_socket()) < 0)
		return -1;

	init_sockaddr_in(AF_INET, ipstr, pot, &raddr);

	//连接服务端
	while(1) {
		//创建套结字
		ret = connect(sockfd, (struct sockaddr*)&raddr, sizeof(raddr));
		//如果服务端未启动,自动重连
		if(ret != 0) {
			continue;
		}
		if(ret != 0) {
			perror("connect failed:");	
			//关閉所有打开的串口 
			closesocket(sockfd);
			return -1;
		}
		break;
	}

	return sockfd;
}


//读取套結字发来的消息
int read_socket(int sockfd, char *des, int len)
{
	int ret;

	memset(des , '\0', len);

	ret = recv(sockfd, des, len, MSG_WAITALL);
	if(ret < 0) {
	perror("read: ");	
			return -1;
	} 
		

	return ret; 
}

//通过套結字发送消息
int write_socket(int fd, char *src, int len)
{
	return send(fd, src, len, 0);
}

//关閉套結字
void close_socket(int fd)
{
	closesocket(fd);
}