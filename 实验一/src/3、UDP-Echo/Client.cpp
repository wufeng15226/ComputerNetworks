/* UDPClient.cpp */

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <time.h>
#pragma warning(disable : 4996)
#define	BUFLEN		2000                  // ��������С
#define WSVERS		MAKEWORD(2, 2)        // ָ���汾2.2 
#pragma comment(lib,"ws2_32.lib")         // ����winsock 2.2 Llibrary

void
main(int argc, char* argv[])
{
	char str1[BUFLEN] = "127.0.0.1";
	char* host = str1;	    /* server IP Address to connect */
	char str2[BUFLEN] = "50500";
	char* service = str2;  	    /* server port to connect       */
	struct sockaddr_in toAddr;	        /* an Internet endpoint address	*/
	int toAddrsize = sizeof(toAddr);
	char	buf[BUFLEN + 1];   		    /* buffer for one line of text	*/
	SOCKET	sock;		  	            /* socket descriptor	    	*/
	int	cc;			                    /* recv character count		    */
	char* pts;			            /* pointer to time string	    */
	time_t	now;			            /* current time			        */

	WSADATA wsadata;
	WSAStartup(WSVERS, &wsadata);       /* ����ĳ�汾Socket��DLL        */

	sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	memset(&toAddr, 0, sizeof(toAddr));
	toAddr.sin_family = AF_INET;
	toAddr.sin_port = htons((u_short)atoi(service));    //atoi����asciiת��Ϊint. htons��������(host)ת��Ϊ������(network), s--short
	toAddr.sin_addr.s_addr = inet_addr(host);           //���hostΪ��������Ҫ���ú���gethostbyname������ת��ΪIP��ַ


	memset(buf, 0, BUFLEN);
	printf("������Ҫ���͵���Ϣ��");
	scanf("%s", buf);
	getchar();

	cc = sendto(sock, buf, BUFLEN, 0, (SOCKADDR*)&toAddr, sizeof(toAddr));
	if (cc == SOCKET_ERROR) {
		printf("����ʧ�ܣ�����ţ�%d\n", WSAGetLastError());
	}
	else {
		//printf("���ͳɹ�!\r\n");
	}

	cc = recvfrom(sock, buf, BUFLEN, 0, (SOCKADDR*)&toAddr, &toAddrsize);           // �ڶ�������ָ�򻺳���������������Ϊ��������С(�ֽ���)�����ĸ�����һ������Ϊ0������ֵ:(>0)���յ����ֽ���,(=0)�Է��ѹر�,(<0)���ӳ���
	if (cc == SOCKET_ERROR)                          // ����������ر��׽���sock
		printf("Error: %d.\n", GetLastError());
	else if (cc == 0) {                             // �Է������ر�
		printf("Server closed!", buf);
	}
	else if (cc > 0) {
		buf[cc] = '\0';	                       // ensure null-termination
		printf("\n�յ�����Ϣ��\n%s\n", buf);                         // ��ʾ�����յ��ַ���
	}
	closesocket(sock);
	WSACleanup();       	          /* ж��ĳ�汾��DLL */

	printf("��������˳�...");
	getchar();

}