/* TCPClient.cpp */
#pragma warning( disable : 4996)

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <process.h>


#define	BUFLEN		2000                  // 缓冲区大小
#define WSVERS		MAKEWORD(2, 0)        // 指明版本2.0 
#pragma comment(lib,"ws2_32.lib")         // 使用winsock 2.0 Llibrary

unsigned __stdcall rec(void* ssock)
{
    char	buf[BUFLEN + 1];   		        /* buffer for one line of text	*/
    int cc;
    SOCKET sock = (SOCKET) ssock;
    while (1)
    {
        cc = recv(sock, buf, BUFLEN, 0);                // cc为接收到的字符的个数(>0)或对方已关闭(=0)或连接出错(<0)
        if (cc == SOCKET_ERROR || cc == 0)
        {
            //printf("Error: %d.\n", GetLastError());     //出错。其后必须关闭套接字sock。
            break;
        }
        else if (cc > 0) {
            buf[cc] = '\0';	                       // ensure null-termination
            printf("\r%s", buf);                         // 显示所接收的字符串
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
    WSAStartup(WSVERS, &wsadata);						  //加载winsock library。WSVERS为请求的版本，wsadata返回系统实际支持的最高版本

    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	  //创建套接字，参数：因特网协议簇(family)，流套接字，TCP协议
                                                          //返回：要监听套接字的描述符或INVALID_SOCKET

    memset(&sin, 0, sizeof(sin));						  // 从&sin开始的长度为sizeof(sin)的内存清0
    sin.sin_family = AF_INET;							  // 因特网地址簇
    sin.sin_addr.s_addr = inet_addr(host);                // 服务器IP地址(32位)
    sin.sin_port = htons((u_short)atoi(service));         // 服务器端口号  
    connect(sock, (struct sockaddr*) & sin, sizeof(sin));  // 连接到服务器
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
    closesocket(sock);                             // 关闭监听套接字
    WSACleanup();                                  // 卸载winsock library

    printf("按回车键继续...");
    getchar();										// 等待任意按键
}

