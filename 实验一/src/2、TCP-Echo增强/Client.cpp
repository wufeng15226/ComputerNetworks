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
#define HOST "127.0.0.1"
#define BUFFSIZE 200
int main(int argc, char** argv)
{
	WSADATA wsadata;
	WSAStartup(WSVERS, &wsadata);						// ����winsock library��SVERSָ������ʹ�õİ汾��wsadata����ϵͳʵ��֧�ֵ���߰汾

	SOCKET sock;								
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	// �����׽��֣�������������Э���(family)���������׽��֣�TCPЭ�飨�������0Ҳ�У�
														// ���أ�Ҫ�����׽��ֵ���������INVALID_SOCKET
														// sockaddr_in��sockaddr���ߵ�ռ�õ��ڴ��С��һ�µģ���˿��Ի���ת���������������˵�����ǲ�������
	struct  sockaddr_in sin;							// ��sockaddr_in���������Ա��������sockaddr��ͨ�á��������size����ʵ����Ҫsockaddr�Ĵ�С����Ϊ����ֱ��ȡ��sockaddr_in�Ĵ�С
														// �����ϻὫ�Ȳ���sockaddr_in����ת��Ϊsockaddr
	memset(&sin, 0, sizeof(sin));						// ��sin��0
	sin.sin_family = AF_INET;							// ��������ַ��(INET-Internet)
	sin.sin_addr.s_addr = inet_addr(HOST);				// ����IP��ַ
	sin.sin_port = htons((u_short)atoi(SERVICE));		// �����Ķ˿ںš�htons--������������(host to network��s-short 16λ)������MAKEWORD
	int ret = connect(sock, (struct sockaddr*) & sin, sizeof(sin));  // ���ӵ����������ڶ�������ָ���ŷ�������ַ�Ľṹ������������Ϊ�ýṹ�Ĵ�С������ֵΪ0ʱ��ʾ�޴�����������SOCKET_ERROR��ʾ����Ӧ�ó����ͨ��WSAGetLastError()��ȡ��Ӧ������롣

	char buffer[BUFFSIZE];
	memset(buffer, 0, BUFFSIZE);
	printf("������Ҫ���͵���Ϣ��");
	scanf("%s", buffer);
	getchar();
	int cc = send(sock, buffer, strlen(buffer), 0);
	if (cc == SOCKET_ERROR)                          // ����������ر��׽���sock
		printf("Error: %d.\n", GetLastError());
	else if (cc == 0) {                             // �Է������ر�
		printf("Server closed!", buffer);
	}

	cc = recv(sock, buffer, BUFFSIZE, 0);           // �ڶ�������ָ�򻺳���������������Ϊ��������С(�ֽ���)�����ĸ�����һ������Ϊ0������ֵ:(>0)���յ����ֽ���,(=0)�Է��ѹر�,(<0)���ӳ���
	if (cc == SOCKET_ERROR)                          // ����������ر��׽���sock
		printf("Error: %d.\n", GetLastError());
	else if (cc == 0) {                             // �Է������ر�
		printf("Server closed!", buffer);
	}
	else if (cc > 0) {
		buffer[cc] = '\0';	                       // ensure null-termination
		printf("\n�յ�����Ϣ��\n%s\n", buffer);                         // ��ʾ�����յ��ַ���
	}
	printf("���س���������\n");
	getchar();

	closesocket(sock);								    // �رռ����׽���
	
	WSACleanup();										// ж��winsock library
	return 0;
}