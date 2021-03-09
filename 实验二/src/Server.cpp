/* TCPServer.cpp - main */
#pragma warning( disable : 4996)
#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <time.h>
#include <process.h>
#include "conio.h"
#define CLLEN 100
#define QLEN 5
#define BUFFSIZE 100
#define	WSVERS	MAKEWORD(2, 0)
#pragma comment(lib,"ws2_32.lib")  //使用winsock 2.2 library

typedef struct {
	int pos;
	sockaddr_in from;
}argm;
CRITICAL_SECTION cs;		           // 临界区。每个时刻只有一个线程可以进入临界区
SOCKET arr[CLLEN];

unsigned __stdcall client(void* p)
{
	int pos = (*(argm*)p).pos;
	sockaddr_in from = (*(argm*)p).from;
	char buffer[BUFFSIZE] = "Enter";
	int cc;
	char tem[BUFFSIZE];
	memset(tem, 0, BUFFSIZE);
	time_t now = time(0);
	char* curr = ctime(&now);
	sprintf(tem, "ip：%s port：%d\ntime：%smessage：%s\n\n>>",
		inet_ntoa(from.sin_addr), from.sin_port, curr, buffer);

	EnterCriticalSection(&cs);     // 等待进入临界区
	printf("\rip：%s port：%d\ntime：%smessage：%s\n\n>>",
		inet_ntoa(from.sin_addr), from.sin_port, curr, buffer);
	LeaveCriticalSection(&cs);     // 离开临界区

	for (int i = 0; i < CLLEN; i++)
	{
		if (arr[i] != NULL)
		{
			cc = send(arr[i], tem, strlen(tem), 0);
		}
	}
	while (1)
	{
		cc = recv(arr[pos], buffer, BUFFSIZE, 0);
		if (cc == SOCKET_ERROR){                          // 出错。其后必须关闭套接字sock
			//printf("Error: %d.\n", GetLastError());
			break;
		}else if (cc == 0) {                             // 对方正常关闭
			printf("Server closed!", buffer);
			break;
		}
		else if (cc > 0)
		{
			buffer[cc] = '\0';	                       // ensure null-termination
			
			now = time(0);
			curr = ctime(&now);
			
			sprintf(tem, "ip：%s port：%d\ntime：%smessage：%s\n\n>>",
				inet_ntoa(from.sin_addr), from.sin_port, curr, buffer);

			EnterCriticalSection(&cs);     // 等待进入临界区
			printf("\rip：%s port：%d\ntime：%smessage：%s\n\n>>",
				inet_ntoa(from.sin_addr), from.sin_port, curr, buffer);
			LeaveCriticalSection(&cs);     // 离开临界区

			for (int i = 0; i < CLLEN; i++)
			{
				if (arr[i] != NULL)
				{
					cc = send(arr[i], tem, strlen(tem), 0);
				}
			}
		}
	}
	strcpy(buffer, "Leave");
	sprintf(tem, "ip：%s port：%d\ntime：%smessage：%s\n\n>>",
		inet_ntoa(from.sin_addr), from.sin_port, curr, buffer);

	EnterCriticalSection(&cs);     // 等待进入临界区
	printf("\rip：%s port：%d\ntime：%smessage：%s\n\n>>",
		inet_ntoa(from.sin_addr), from.sin_port, curr, buffer);
	LeaveCriticalSection(&cs);     // 离开临界区

	for (int i = 0; i < CLLEN; i++)
	{
		if (arr[i] != NULL)
		{
			cc = send(arr[i], tem, strlen(tem), 0);
		}
	}
	fflush(stdout);
	(void)closesocket(arr[pos]);                              // 关闭连接套接字
	arr[pos] = NULL;
	return 0;
}

unsigned __stdcall server(void* p)
{
	sockaddr_in from = *(sockaddr_in*)p;
	char buffer[BUFFSIZE];
	memset(buffer, 0, BUFFSIZE);
	int cc;
	printf(">>");
	while (1)
	{
		scanf("%[^\n]", buffer);
		if (!strcmp(buffer, "")) break;
		getchar();
		char tem[BUFFSIZE];
		memset(tem, 0, BUFFSIZE);
		time_t now = time(0);
		char* curr = ctime(&now);

		sprintf(tem, "ip：%s port：%d\ntime：%smessage：%s\n\n>>",
			inet_ntoa(from.sin_addr), from.sin_port, curr, buffer);

		EnterCriticalSection(&cs);     // 等待进入临界区
		printf("\rip：%s port：%d\ntime：%smessage：%s\n\n>>",
			inet_ntoa(from.sin_addr), from.sin_port, curr, buffer);
		LeaveCriticalSection(&cs);     // 离开临界区

		for (int i = 0; i < CLLEN; i++)
		{
			if (arr[i] != NULL)
			{
				cc = send(arr[i], tem, strlen(tem), 0);
			}
		}
	}

	return 0;
}

int main(int argc, char* argv[])
/* argc: 命令行参数个数， 例如：C:\> TCPServer 8080
					 argc=2 argv[0]="TCPServer",argv[1]="8080" */
{
	SOCKET	msock;		    /* master & slave sockets	      */
	WSADATA wsadata;
	char tem[6] = "50500";
	char* service = tem;
	struct  sockaddr_in sin;	    /* an Internet endpoint address		*/
	int	    alen;			        /* from-address length		        */
	char* pts;			        /* pointer to time string	        */
	time_t	now;			        /* current time			            */


	WSAStartup(WSVERS, &wsadata);						// 加载winsock library。WSVERS指明请求使用的版本。wsadata返回系统实际支持的最高版本
	msock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	// 创建套接字，参数：因特网协议簇(family)，流套接字，TCP协议
														// 返回：要监听套接字的描述符或INVALID_SOCKET

	memset(&sin, 0, sizeof(sin));						// 从&sin开始的长度为sizeof(sin)的内存清0
	sin.sin_family = AF_INET;							// 因特网地址簇(INET-Internet)
	sin.sin_addr.s_addr = INADDR_ANY;					// 监听所有(接口的)IP地址。
	sin.sin_port = htons((u_short)atoi(service));		// 监听的端口号。atoi--把ascii转化为int，htons--主机序到网络序(16位)
	bind(msock, (struct sockaddr*) & sin, sizeof(sin));  // 绑定监听的IP地址和端口号

	listen(msock, QLEN);                                   // 等待建立连接的队列长度为5
	
	
	InitializeCriticalSection(&cs);   // 临界区初始化
	HANDLE thr[CLLEN];
	unsigned int thrid[CLLEN];
	argm ag[CLLEN];
	unsigned int ct = 0;
	for (int i = 0; i < CLLEN; i++)
	{
		thr[i] = NULL;
		ag[i].pos = i;
	}
	thr[0] = (HANDLE)_beginthreadex(NULL, 0, &server, (void*)&sin, 0, thrid);
	while (!_kbhit()) { 		                             // 检测是否有按键
		alen = sizeof(struct sockaddr);                   // 取到地址结构的长度
		arr[ct] = accept(msock, (struct sockaddr*) & (ag[ct].from)  , &alen); // 如果有新的连接请求，返回连接套接字，否则，被阻塞。fsin包含客户端IP地址和端口号
		thr[ct+1] = (HANDLE)_beginthreadex(NULL, 0, &client, (void*)(ag + ct), 0, thrid + ct);
		ct++;
	}

	for (int i = 0; i < CLLEN; i++)
	{
		if(thr[i]!=NULL) WaitForSingleObject(thr[i], INFINITE);
	}

	DeleteCriticalSection(&cs);  // 删除临界区
	for (int i = 0; i < CLLEN; i++)
	{
		if (thr[i] != NULL) CloseHandle(thr[i]);
	}
	(void)closesocket(msock);                                 // 关闭监听套接字
	WSACleanup();                                             // 卸载winsock library
}

