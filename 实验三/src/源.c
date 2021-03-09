#pragma warning( disable : 4996)

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <process.h>
#include <ws2tcpip.h>

#define WSVERS AKEWORD(2, 0)        // 指明版本2.0 
#pragma comment(lib,"ws2_32.lib")         // 使用winsock 2.0 Llibrary
#define	BUFLEN (10 << 25)                  // 缓冲区大小

char buff[BUFLEN + 1];

unsigned __stdcall rec(void* ssock)
{
	int cc;
	SOCKET sock = (SOCKET)ssock;
	while (1)
	{
		cc = recv(sock, buff, BUFLEN, 0);                // cc为接收到的字符的个数(>0)或对方已关闭(=0)或连接出错(<0)
		if (cc == SOCKET_ERROR || cc == 0)
		{
			//printf("Error: %d.\n", GetLastError());     //出错。其后必须关闭套接字sock。
			break;
		}
		else if (cc > 0) {
			buff[cc] = '\0';	                       // ensure null-termination
			//printf("%s\n", buff);
			int pos = 0;
			while (pos<cc)
			{
				printf("%c", buff[pos]);
				pos++;
			}
		}
	}
	return 0;
}


int main(int argc, char** argv)
{
	if (argc != 2)
		return 0;
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 0), &wsadata);
	struct addrinfo* host;
	getaddrinfo(argv[1], "http", NULL, &host);//默认http，仅需输入一个参数
	SOCKET sock = socket(host->ai_family, host->ai_socktype, host->ai_protocol);
	//struct sockaddr_in sin;	            /* an Internet endpoint address	*/
	//char service[3] = "80";
	//memset(&sin, 0, sizeof(sin));						  // 从&sin开始的长度为sizeof(sin)的内存清0
	//sin.sin_family = AF_INET;							  // 因特网地址簇
	//sin.sin_addr.s_addr = inet_addr(host->ai_addr->sa_data);                // 服务器IP地址(32位)
	//sin.sin_port = htons((u_short)atoi(service));         // 服务器端口号  
	//if (connect(sock, (struct sockaddr*) & sin, sizeof(sin))) printf("Connect Success!\n");
	if (!connect(sock, host->ai_addr, host->ai_addrlen)) printf("Connect Success!\n");

	HANDLE thr = (HANDLE)_beginthreadex(NULL, 0, &rec, (void*)sock, 0, NULL);

	char buf[10000];
	char buffer[1000];
	const char str[3] = "\r\n";
	char* cc;
	int len = 0;
	while (1)
	{
		cc = gets_s(buffer, 100);
		if (!cc) break;
		strcpy(buf + len, buffer);
		len += strlen(buffer);
		strcpy(buf + len, str);
		len += 2;
	}

	//printf("%s\n", buf);
	send(sock, buf, len, 0);


	WaitForSingleObject(thr, INFINITE);
	CloseHandle(thr);
	shutdown(sock, SD_BOTH);
	closesocket(sock);
	freeaddrinfo(host);
	WSACleanup();
	return 0;
}