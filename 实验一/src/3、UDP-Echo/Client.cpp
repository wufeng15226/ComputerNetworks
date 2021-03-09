/* UDPClient.cpp */

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <time.h>
#pragma warning(disable : 4996)
#define	BUFLEN		2000                  // 缓冲区大小
#define WSVERS		MAKEWORD(2, 2)        // 指明版本2.2 
#pragma comment(lib,"ws2_32.lib")         // 加载winsock 2.2 Llibrary

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
	WSAStartup(WSVERS, &wsadata);       /* 启动某版本Socket的DLL        */

	sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	memset(&toAddr, 0, sizeof(toAddr));
	toAddr.sin_family = AF_INET;
	toAddr.sin_port = htons((u_short)atoi(service));    //atoi：把ascii转化为int. htons：主机序(host)转化为网络序(network), s--short
	toAddr.sin_addr.s_addr = inet_addr(host);           //如果host为域名，需要先用函数gethostbyname把域名转化为IP地址


	memset(buf, 0, BUFLEN);
	printf("请输入要发送的消息：");
	scanf("%s", buf);
	getchar();

	cc = sendto(sock, buf, BUFLEN, 0, (SOCKADDR*)&toAddr, sizeof(toAddr));
	if (cc == SOCKET_ERROR) {
		printf("发送失败，错误号：%d\n", WSAGetLastError());
	}
	else {
		//printf("发送成功!\r\n");
	}

	cc = recvfrom(sock, buf, BUFLEN, 0, (SOCKADDR*)&toAddr, &toAddrsize);           // 第二个参数指向缓冲区，第三个参数为缓冲区大小(字节数)，第四个参数一般设置为0，返回值:(>0)接收到的字节数,(=0)对方已关闭,(<0)连接出错
	if (cc == SOCKET_ERROR)                          // 出错。其后必须关闭套接字sock
		printf("Error: %d.\n", GetLastError());
	else if (cc == 0) {                             // 对方正常关闭
		printf("Server closed!", buf);
	}
	else if (cc > 0) {
		buf[cc] = '\0';	                       // ensure null-termination
		printf("\n收到的消息：\n%s\n", buf);                         // 显示所接收的字符串
	}
	closesocket(sock);
	WSACleanup();       	          /* 卸载某版本的DLL */

	printf("按任意键退出...");
	getchar();

}