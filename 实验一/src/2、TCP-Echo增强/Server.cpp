#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>
#include "conio.h"				   // ��������Dos�����ϵͳ
#pragma comment(lib,"ws2_32.lib")  // ʹ��winsock 2.2 library
#pragma warning(disable : 4996)
#define	WSVERS	MAKEWORD(2, 2)		// MAKEWORD��������һ���������紫�䣨С�ˣ�����2B��word������Ĳ�����ʾ�����WSAStartup��ʹ�ð汾��2.2������2.0Ҳ�У�
#define SERVICE "50500"
#define QUSIZE 5
#define BUFFSIZE 200

int main(int argv,char** argc)
{

	WSADATA wsadata;
	WSAStartup(WSVERS, &wsadata);						// ����winsock library��SVERSָ������ʹ�õİ汾��wsadata����ϵͳʵ��֧�ֵ���߰汾

	SOCKET	msock, ssock;								// master & slave sockets��Ϊ���������ṩ֧�֣�����ֻ��1��1
	msock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	// �����׽��֣�������������Э���(family)���������׽��֣�TCPЭ�飨�������0Ҳ�У�
														// ���أ�Ҫ�����׽��ֵ���������INVALID_SOCKET

														// sockaddr_in��sockaddr���ߵ�ռ�õ��ڴ��С��һ�µģ���˿��Ի���ת���������������˵�����ǲ�������
	struct  sockaddr_in sin;							// ��sockaddr_in���������Ա��������sockaddr��ͨ�á��������size����ʵ����Ҫsockaddr�Ĵ�С����Ϊ����ֱ��ȡ��sockaddr_in�Ĵ�С
														// �����ϻὫ�Ȳ���sockaddr_in����ת��Ϊsockaddr
	memset(&sin, 0, sizeof(sin));						// ��sin��0
	sin.sin_family = AF_INET;							// ��������ַ��(INET-Internet)
	sin.sin_addr.s_addr = INADDR_ANY;					// ��������(�ӿڵ�)IP��ַ��
	sin.sin_port = htons((u_short)atoi(SERVICE));		// �����Ķ˿ںš�htons--������������(host to network��s-short 16λ)������MAKEWORD
	bind(msock, (struct sockaddr*) & sin, sizeof(sin)); // �󶨼�����IP��ַ�Ͷ˿ں�

	listen(msock, QUSIZE);								// ���м������ڶ�������Ϊ�������г��ȣ��·�ѭ���Ǽ����ľ��������Ҳ�Ǻ���
	printf("��������������\n\n");
	while (!_kbhit())									// ������conio.h�����а���������ѭ��
	{	
		struct  sockaddr_in fsin;
		int hd_sz = sizeof(fsin);
		ssock = accept(msock, (struct sockaddr*) & fsin, &hd_sz); // �������ӣ��������׽��֣�fsin��ŵ��ǿͻ��˵�sockaddr��hd_szΪ��С��ָ��
		
		char buffer[BUFFSIZE];
		memset(buffer, 0, BUFFSIZE);
		int cc = recv(ssock, buffer, BUFFSIZE, 0);
		if (cc == SOCKET_ERROR)                          // ����������ر��׽���sock
			printf("Error: %d.\n", GetLastError());
		else if (cc == 0) {                             // �Է������ر�
			printf("Server closed!", buffer);
		}
		else if (cc > 0) {
			buffer[cc] = '\0';	                       // ensure null-termination


			char tem[BUFFSIZE];
			memset(tem, 0, BUFFSIZE);
			time_t now = time(0);
			char* curr = ctime(&now);
			unsigned int ip = fsin.sin_addr.S_un.S_addr;

			printf("�յ�����Ϣ��\n���ݣ�%s\n�յ�ʱ�䣺%s�ͻ���IP��ַ��%d.%d.%d.%d\n�ͻ��˶˿ںţ�%d\n",
				buffer, curr, (ip << 24) >> 24, (ip << 16) >> 24, (ip << 8) >> 24, ip >> 24, fsin.sin_port);
			sprintf(tem, "�յ�����Ϣ��\n���ݣ�%s\n�յ�ʱ�䣺%s�ͻ���IP��ַ��%d.%d.%d.%d\n�ͻ��˶˿ںţ�%d\n",
				buffer, curr, (ip << 24) >> 24, (ip << 16) >> 24, (ip << 8) >> 24, ip >> 24, fsin.sin_port);
			cc = send(ssock, tem, strlen(tem), 0);		
			// �ڶ�������ָ���ͻ�����������������ΪҪ���͵��ֽ��������ĸ�����һ����0������ֵ��>=0 ʵ�ʷ��͵��ֽ�����0 �Է������رգ�SOCKET_ERROR ����
		}

		closesocket(ssock);								// �ر��׽���
	}

	closesocket(msock);								    // �رռ����׽���

	WSACleanup();										// ж��winsock library

	return 0;
}