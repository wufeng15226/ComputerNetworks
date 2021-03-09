/* UDPServer.cpp */

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <time.h>
#include "conio.h"
#pragma warning(disable : 4996)

#define	BUFLEN		2000                  // ��������С
#define WSVERS		MAKEWORD(2, 2)        // ָ���汾2.2 
#pragma comment(lib,"ws2_32.lib")         // ����winsock 2.2 Llibrary

/*------------------------------------------------------------------------
 * main - TCP client for DAYTIME service
 *------------------------------------------------------------------------
 */
void
main(int argc, char* argv[])
{
	char str1[BUFLEN] = "127.0.0.1";
	char* host = str1;	    /* server IP Address to connect */
	char str2[BUFLEN] = "50500";
	char* service = str2;  	    /* server port to connect       */
	struct sockaddr_in sin;	        /* an Internet endpoint address	*/
	struct sockaddr_in from;        /* sender address               */
	int    fromsize = sizeof(from);
	char   buf[BUFLEN + 1];   	    /* buffer for one line of text	*/
	SOCKET	sock;		  	        /* socket descriptor	    	*/
	int	cc;			                /* recv character count		    */

	WSADATA wsadata;
	WSAStartup(WSVERS, &wsadata);   /* ����winsock library��WSVERSΪ����汾,wsadata����ϵͳʵ��֧�ֵ���߰汾��    */
	sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP); // ����UDP�׽���, ������������Э���(family)�����ݱ��׽��֣�UDPЭ��ţ� ���أ�Ҫ�����׽��ֵ���������INVALID_SOCKET
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;                     // ��(����)���еĽӿڡ�
	sin.sin_port = htons((u_short)atoi(service));         // ��ָ���ӿڡ�atoi--��asciiת��Ϊint��htons -- ������(host)ת��Ϊ������(network), Ϊshort���͡� 
	bind(sock, (struct sockaddr*) & sin, sizeof(sin));     // �󶨱��ض˿ںţ��ͱ���IP��ַ)

	while (!_kbhit()) {                                    //����Ƿ��а���
		cc = recvfrom(sock, buf, BUFLEN, 0, (SOCKADDR*)&from, &fromsize);  //���տͻ����ݡ����ؽ����ccΪ���յ��ַ�����from�н������ͻ�IP��ַ�Ͷ˿ںš�
		if (cc == SOCKET_ERROR) {
			printf("recvfrom() failed; %d\n", WSAGetLastError());
			break;
		}
		else if (cc == 0)
			break;
		else {
			buf[cc] = '\0';
			char tem[BUFLEN];
			memset(tem, 0, BUFLEN);
			time_t now = time(0);
			char* curr = ctime(&now);

			printf("�ͻ��˵���Ϣ��%s\n�ͻ���IP��ַ��%s\n�ͻ��˶˿ںţ�%d\nʱ�䣺%s",
				buf, inet_ntoa(from.sin_addr), from.sin_port, curr);
			sprintf(tem, "�ͻ��˵���Ϣ��%s\n�ͻ���IP��ַ��%s\n�ͻ��˶˿ںţ�%d\nʱ�䣺%s",
				buf, inet_ntoa(from.sin_addr), from.sin_port, curr);
			cc = sendto(sock, tem, strlen(tem), 0, (SOCKADDR*)&from, fromsize);
		}
	}
	closesocket(sock);
	WSACleanup();       	          /* ж��ĳ�汾��DLL */
	getchar();
}


