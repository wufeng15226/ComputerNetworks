#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>
#include "conio.h"				   // 仅能用于Dos类操作系统
#pragma comment(lib,"ws2_32.lib")  // 使用winsock 2.2 library
#pragma warning(disable : 4996)
#define	WSVERS	MAKEWORD(2, 2)		// MAKEWORD函数创建一个符合网络传输（小端？）的2B的word，这里的参数表示后面的WSAStartup将使用版本号2.2（换成2.0也行）
#define SERVICE "50500"
#define QUSIZE 5
#define BUFFSIZE 100

int main(int argv,char** argc)
{

	WSADATA wsadata;
	WSAStartup(WSVERS, &wsadata);						// 加载winsock library，SVERS指明请求使用的版本。wsadata返回系统实际支持的最高版本

	SOCKET	msock, ssock;								// master & slave sockets，为并发服务提供支持，否则只能1对1
	msock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	// 创建套接字，参数：因特网协议簇(family)，（流）套接字，TCP协议（这个换成0也行）
														// 返回：要监听套接字的描述符或INVALID_SOCKET

														// sockaddr_in与sockaddr二者的占用的内存大小是一致的，因此可以互相转化，从这个意义上说，他们并无区别
	struct  sockaddr_in sin;							// 但sockaddr_in更方便程序员操作，而sockaddr更通用。后面相关size操作实际是要sockaddr的大小，但为方便直接取了sockaddr_in的大小
														// 惯例上会将先操作sockaddr_in，再转化为sockaddr
	memset(&sin, 0, sizeof(sin));						// 将sin清0
	sin.sin_family = AF_INET;							// 因特网地址簇(INET-Internet)
	sin.sin_addr.s_addr = INADDR_ANY;					// 监听所有(接口的)IP地址。
	sin.sin_port = htons((u_short)atoi(SERVICE));		// 监听的端口号。htons--主机序到网络序(host to network，s-short 16位)，类似MAKEWORD
	bind(msock, (struct sockaddr*) & sin, sizeof(sin)); // 绑定监听的IP地址和端口号

	listen(msock, QUSIZE);								// 进行监听，第二个参数为监听队列长度，下方循环是监听的具体操作，也是核心
	printf("服务器已启动！\n\n");
	while (!_kbhit())									// 定义在conio.h，若有按键，结束循环
	{	
		struct  sockaddr_in fsin;
		int hd_sz = sizeof(fsin);
		ssock = accept(msock, (struct sockaddr*) & fsin, &hd_sz); // 建立连接，创建新套接字，fsin存放的是客户端的sockaddr，hd_sz为大小的指针
		
		char buffer[BUFFSIZE];
		memset(buffer, 0, BUFFSIZE);
		int cc = recv(ssock, buffer, BUFFSIZE, 0);
		if (cc == SOCKET_ERROR)                          // 出错。其后必须关闭套接字sock
			printf("Error: %d.\n", GetLastError());
		else if (cc == 0) {                             // 对方正常关闭
			printf("Server closed!", buffer);
		}
		else if (cc > 0) {
			buffer[cc] = '\0';	                       // ensure null-termination
			printf("收到消息：%s\n", buffer);                         // 显示所接收的字符串
		}

		char tem[BUFFSIZE];
		memset(tem, 0, BUFFSIZE);
		time_t now = time(0);		
		char* curr = ctime(&now);
		printf("收到时间：%s\n", curr);
		strcpy(tem, curr);
		int len = strlen(tem);
		tem[len - 1] = '\n';
		tem[len] = '\0';
		strcat(tem, buffer);
		cc = send(ssock, tem, strlen(tem), 0);		// 第二个参数指向发送缓冲区，第三个参数为要发送的字节数，第四个参数一般置0，返回值：>=0 实际发送的字节数，0 对方正常关闭，SOCKET_ERROR 出错。
		
		closesocket(ssock);								// 关闭套接字
	}

	closesocket(msock);								    // 关闭监听套接字

	WSACleanup();										// 卸载winsock library

	return 0;
}