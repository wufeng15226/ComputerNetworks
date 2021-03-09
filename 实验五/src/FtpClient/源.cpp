#pragma warning( disable : 4996)
#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <process.h>
#include <ws2tcpip.h>

#define WSVERS AKEWORD(2, 0)				// 指明版本2.0 
#pragma comment(lib,"ws2_32.lib")			// 使用winsock 2.0 Llibrary

#define	BUFLEN (10 << 24)					// 缓冲区大小
#define FTP_MSG_1 "user net\r\n"
#define FTP_MSG_2 "pass 123456\r\n"
#define FTP_MSG_3 "pasv\r\n"
#define FTP_MSG_4 "quit\r\n"
#define FTP_SG "227"

char argv_3[100];
char buff[BUFLEN + 1];
char data_buff[BUFLEN + 1];

unsigned __stdcall rec_2(void* sock)
{
	int cc;
	SOCKET* ssock = (SOCKET*)sock;
	FILE* file = fopen(argv_3, "wb");
	fclose(file);
	//printf("%s\n", ((agmt*)ag)->argv_3);
	puts("开始下载文件...");
	while (1)
	{
		cc = recv(*ssock, data_buff, BUFLEN, 0);         // cc为接收到的字符的个数(>0)或对方已关闭(=0)或连接出错(<0)
		//printf("%s\n", data_buff);
		if (cc == SOCKET_ERROR || cc == 0)
		{
			//printf("Error: %d.\n", GetLastError());     //出错。其后必须关闭套接字sock。
			break;
		}
		else if (cc > 0) {
			FILE* file = fopen(argv_3, "ab");
			if (!file) puts("FILE OPEN ERROR!\n");
			fwrite(data_buff, 1, cc, file);
			fclose(file);
		}
	}
	puts("下载结束");
	return 0;
}

void dataConn()
{
	char* pos1 = strchr(buff, '(');
	int ip1, ip2, ip3, ip4, port1, port2;
	sscanf(pos1 + 1, "%d,%d,%d,%d,%d,%d", &ip1, &ip2, &ip3, &ip4, &port1, &port2);
	char rst[100] = {};
	sprintf(rst, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
	struct addrinfo* host;
	char port[100] = {};
	itoa(port1 * 0x100 + port2, port, 10);
	getaddrinfo(rst, port, NULL, &host);
	SOCKET sock = socket(host->ai_family, host->ai_socktype, host->ai_protocol);
	if (connect(sock, host->ai_addr, host->ai_addrlen)) puts("Date Connect Error!");
	//printf("%s\n", ag.argv_3);
	HANDLE thr = (HANDLE)_beginthreadex(NULL, 0, &rec_2, (void*)&sock, 0, NULL);
	WaitForSingleObject(thr, INFINITE);
	CloseHandle(thr);
	shutdown(sock, SD_BOTH);
	closesocket(sock);
	freeaddrinfo(host);
}


unsigned __stdcall rec_1(void* sock)
{
	int cc;
	SOCKET* ssock = (SOCKET*)sock;
	while (1)
	{
		cc = recv(*ssock, buff, BUFLEN, 0);             // cc为接收到的字符的个数(>0)或对方已关闭(=0)或连接出错(<0)
		if (cc == SOCKET_ERROR || cc == 0)
		{
			//printf("Error: %d.\n", GetLastError());    //出错。其后必须关闭套接字sock。
			break;
		}
		else if (cc > 0) {
			buff[cc] = '\0';	                       // ensure null-termination
			int pos = 0;
			while (pos < cc)
			{
				printf("%c", buff[pos]);
				pos++;
			}
			if(strchr(buff, '(')) dataConn();
		}
	}
	return 0;
}


int main(int argc, char** argv)
{
	if (argc != 4) return 0;
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 0), &wsadata);
	struct addrinfo* host;
	getaddrinfo(argv[1], "ftp", NULL, &host);											//默认ftp
	SOCKET sock = socket(host->ai_family, host->ai_socktype, host->ai_protocol);
	if (connect(sock, host->ai_addr, host->ai_addrlen)) printf("Connect Error!\n");
	strcpy(argv_3,argv[3]);
	HANDLE thr = (HANDLE)_beginthreadex(NULL, 0, &rec_1, (void*)&sock, 0, NULL);

	char buf[100] = FTP_MSG_1;
	printf("%s", buf);
	send(sock, buf, strlen(buf), 0);
	strcpy(buf, FTP_MSG_2);
	printf("%s", buf);
	send(sock, buf, strlen(buf), 0);
	strcpy(buf, FTP_MSG_3);
	printf("%s", buf);
	send(sock, buf, strlen(buf), 0);
	sprintf(buf, "retr %s\r\n", argv[2]);
	printf("%s", buf);
	send(sock, buf, strlen(buf), 0);
	strcpy(buf, FTP_MSG_4);
	printf("%s", buf);
	send(sock, buf, strlen(buf), 0);


	WaitForSingleObject(thr, INFINITE);
	CloseHandle(thr);
	shutdown(sock, SD_BOTH);
	closesocket(sock);
	freeaddrinfo(host);
	WSACleanup();
	return 0;
}