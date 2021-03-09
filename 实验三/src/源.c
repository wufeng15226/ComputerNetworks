#pragma warning( disable : 4996)

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <process.h>
#include <ws2tcpip.h>

#define WSVERS AKEWORD(2, 0)        // ָ���汾2.0 
#pragma comment(lib,"ws2_32.lib")         // ʹ��winsock 2.0 Llibrary
#define	BUFLEN (10 << 25)                  // ��������С

char buff[BUFLEN + 1];

unsigned __stdcall rec(void* ssock)
{
	int cc;
	SOCKET sock = (SOCKET)ssock;
	while (1)
	{
		cc = recv(sock, buff, BUFLEN, 0);                // ccΪ���յ����ַ��ĸ���(>0)��Է��ѹر�(=0)�����ӳ���(<0)
		if (cc == SOCKET_ERROR || cc == 0)
		{
			//printf("Error: %d.\n", GetLastError());     //����������ر��׽���sock��
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
	getaddrinfo(argv[1], "http", NULL, &host);//Ĭ��http����������һ������
	SOCKET sock = socket(host->ai_family, host->ai_socktype, host->ai_protocol);
	//struct sockaddr_in sin;	            /* an Internet endpoint address	*/
	//char service[3] = "80";
	//memset(&sin, 0, sizeof(sin));						  // ��&sin��ʼ�ĳ���Ϊsizeof(sin)���ڴ���0
	//sin.sin_family = AF_INET;							  // ��������ַ��
	//sin.sin_addr.s_addr = inet_addr(host->ai_addr->sa_data);                // ������IP��ַ(32λ)
	//sin.sin_port = htons((u_short)atoi(service));         // �������˿ں�  
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