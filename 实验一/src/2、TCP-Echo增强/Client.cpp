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
#define HOST "127.0.0.1"
#define BUFFSIZE 200
int main(int argc, char** argv)
{
	WSADATA wsadata;
	WSAStartup(WSVERS, &wsadata);						// 加载winsock library，SVERS指明请求使用的版本。wsadata返回系统实际支持的最高版本

	SOCKET sock;								
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	// 创建套接字，参数：因特网协议簇(family)，（流）套接字，TCP协议（这个换成0也行）
														// 返回：要监听套接字的描述符或INVALID_SOCKET
														// sockaddr_in与sockaddr二者的占用的内存大小是一致的，因此可以互相转化，从这个意义上说，他们并无区别
	struct  sockaddr_in sin;							// 但sockaddr_in更方便程序员操作，而sockaddr更通用。后面相关size操作实际是要sockaddr的大小，但为方便直接取了sockaddr_in的大小
														// 惯例上会将先操作sockaddr_in，再转化为sockaddr
	memset(&sin, 0, sizeof(sin));						// 将sin清0
	sin.sin_family = AF_INET;							// 因特网地址簇(INET-Internet)
	sin.sin_addr.s_addr = inet_addr(HOST);				// 设置IP地址
	sin.sin_port = htons((u_short)atoi(SERVICE));		// 监听的端口号。htons--主机序到网络序(host to network，s-short 16位)，类似MAKEWORD
	int ret = connect(sock, (struct sockaddr*) & sin, sizeof(sin));  // 连接到服务器，第二个参数指向存放服务器地址的结构，第三个参数为该结构的大小，返回值为0时表示无错误发生，返回SOCKET_ERROR表示出错，应用程序可通过WSAGetLastError()获取相应错误代码。

	char buffer[BUFFSIZE];
	memset(buffer, 0, BUFFSIZE);
	printf("请输入要发送的消息：");
	scanf("%s", buffer);
	getchar();
	int cc = send(sock, buffer, strlen(buffer), 0);
	if (cc == SOCKET_ERROR)                          // 出错。其后必须关闭套接字sock
		printf("Error: %d.\n", GetLastError());
	else if (cc == 0) {                             // 对方正常关闭
		printf("Server closed!", buffer);
	}

	cc = recv(sock, buffer, BUFFSIZE, 0);           // 第二个参数指向缓冲区，第三个参数为缓冲区大小(字节数)，第四个参数一般设置为0，返回值:(>0)接收到的字节数,(=0)对方已关闭,(<0)连接出错
	if (cc == SOCKET_ERROR)                          // 出错。其后必须关闭套接字sock
		printf("Error: %d.\n", GetLastError());
	else if (cc == 0) {                             // 对方正常关闭
		printf("Server closed!", buffer);
	}
	else if (cc > 0) {
		buffer[cc] = '\0';	                       // ensure null-termination
		printf("\n收到的消息：\n%s\n", buffer);                         // 显示所接收的字符串
	}
	printf("按回车键继续：\n");
	getchar();

	closesocket(sock);								    // 关闭监听套接字
	
	WSACleanup();										// 卸载winsock library
	return 0;
}