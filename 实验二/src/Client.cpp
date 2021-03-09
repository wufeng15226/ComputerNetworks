/* TCPClient.cpp */
#pragma warning( disable : 4996)

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <process.h>


#define	BUFLEN		2000                  // ��������С
#define WSVERS		MAKEWORD(2, 0)        // ָ���汾2.0 
#pragma comment(lib,"ws2_32.lib")         // ʹ��winsock 2.0 Llibrary

unsigned __stdcall rec(void* ssock)
{
    char	buf[BUFLEN + 1];   		        /* buffer for one line of text	*/
    int cc;
    SOCKET sock = (SOCKET) ssock;
    while (1)
    {
        cc = recv(sock, buf, BUFLEN, 0);                // ccΪ���յ����ַ��ĸ���(>0)��Է��ѹر�(=0)�����ӳ���(<0)
        if (cc == SOCKET_ERROR || cc == 0)
        {
            //printf("Error: %d.\n", GetLastError());     //����������ر��׽���sock��
            break;
        }
        else if (cc > 0) {
            buf[cc] = '\0';	                       // ensure null-termination
            printf("\r%s", buf);                         // ��ʾ�����յ��ַ���
        }
    }
    
    return 0;
}

/*------------------------------------------------------------------------
 * main - TCP client for TIME service
 *------------------------------------------------------------------------
 */
void
main(int argc, char* argv[])
{
    //char tem1[20] = "127.0.0.1";
    //char tem1[20] = "103.26.79.35";
    char tem1[20] = "111.230.210.193";

    char* host = tem1;	    /* server IP to connect         */
    char tem2[6] = "50500";

    char* service = tem2;  	    /* server port to connect       */
    struct sockaddr_in sin;	            /* an Internet endpoint address	*/
    SOCKET	sock;		  	            /* socket descriptor	    	*/

    WSADATA wsadata;
    WSAStartup(WSVERS, &wsadata);						  //����winsock library��WSVERSΪ����İ汾��wsadata����ϵͳʵ��֧�ֵ���߰汾

    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	  //�����׽��֣�������������Э���(family)�����׽��֣�TCPЭ��
                                                          //���أ�Ҫ�����׽��ֵ���������INVALID_SOCKET

    memset(&sin, 0, sizeof(sin));						  // ��&sin��ʼ�ĳ���Ϊsizeof(sin)���ڴ���0
    sin.sin_family = AF_INET;							  // ��������ַ��
    sin.sin_addr.s_addr = inet_addr(host);                // ������IP��ַ(32λ)
    sin.sin_port = htons((u_short)atoi(service));         // �������˿ں�  
    connect(sock, (struct sockaddr*) & sin, sizeof(sin));  // ���ӵ�������
    unsigned int thrid;
    HANDLE thr = (HANDLE)_beginthreadex(NULL, 0, &rec, (void*) sock, 0, &thrid);

    char buffer[BUFLEN];
    memset(buffer, 0, BUFLEN);
    int cc = 0;
    while (strcmp(buffer,"exit"))
    {
        //printf(">>");
        scanf("%[^\n]", buffer);
        getchar();
        cc = send(sock, buffer, strlen(buffer), 0);
    }
    
    CloseHandle(thr);
    closesocket(sock);                             // �رռ����׽���
    WSACleanup();                                  // ж��winsock library

    printf("���س�������...");
    getchar();										// �ȴ����ⰴ��
}

