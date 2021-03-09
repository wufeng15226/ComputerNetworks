﻿#pragma warning(disable : 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>
#include <process.h>
#include <sys/stat.h>
#include "conio.h"				   // 仅能用于Dos类操作系统
#pragma comment(lib,"ws2_32.lib")  // 使用winsock 2.2 library
#define	WSVERS	MAKEWORD(2, 2)		// MAKEWORD函数创建一个符合网络传输（小端？）的2B的word，这里的参数表示后面的WSAStartup将使用版本号2.2（换成2.0也行）
#define SERVICE "50500"
#define QUSIZE 2
#define BUFFSIZE (10 << 24)
#define RDIR "rdir"
#define CHAT "chat"
#define SEND "send"
#define QUIT "quit"
#define MAX_FILE_NAME 200
#define MAX_FILE_TYPE 10
#define PDF "pdf"
#define TXT "txt"
#define JPG "jpg"
#define PNG "png"
#define PPTX "pptx"
#define PDF_Struct int
#define TXT_Struct int
#define JPG_Struct int
#define PNG_Struct int
#define PPTX_Struct int

typedef struct {
	char fileName[MAX_FILE_NAME];
	char fileType[MAX_FILE_TYPE];
}FileStruct;

char buf[BUFFSIZE];
char file_name_head[MAX_FILE_NAME];
char data_buf[BUFFSIZE];

void app_str(char* str, int num)
{
	strcpy(str, "(");
	char tem[10];
	itoa(num, tem, 10);
	strcat(str, tem);
	strcat(str, ")");
}

void add_str(char* str, char* front_app)
{
	int len = strlen(front_app);
	char tem[100] = {};
	strcpy(tem, front_app);
	tem[len] = '\\';
	tem[len + 1] = '\0';
	strcpy(tem + len + 1, str);
	strcpy(str, tem);
}

void send_file(SOCKET ssock, char* file_name)
{
	int len = 0;
	FileStruct tem;
	strcpy(tem.fileName, file_name);
	char* pos = strchr(file_name, '.');
	strcpy(tem.fileType, pos + 1);
	memcpy(data_buf, &tem, sizeof(FileStruct));
	len += sizeof(FileStruct);
	add_str(file_name, file_name_head);
	long long sz = 0;
	if (!strcmp(tem.fileType, PDF) || !strcmp(tem.fileType, TXT)
	 || !strcmp(tem.fileType, JPG) || !strcmp(tem.fileType, PNG)
	 || !strcmp(tem.fileType, PPTX))
	{
		PDF_Struct size;
		struct stat statbuf;
		if (stat(file_name, &statbuf) != 0)
		{
			printf("File Size Error!\n");
			return;
		}
		size = statbuf.st_size;
		sz = size;
		memcpy(data_buf+len, &size, sizeof(PDF_Struct));
		len += sizeof(PDF_Struct);
	}
	else {
		printf("File Type Error!\n");
		return;
	}
	send(ssock, data_buf, len, 0);
	FILE* file = fopen(file_name, "rb");
	if (!file)
	{
		printf("FILE OPEN ERROR!\n");
		return;
	}
	while (sz>0)
	{
		fread(data_buf, 1, BUFFSIZE>sz?sz:BUFFSIZE, file);
		sz -= send(ssock, data_buf, BUFFSIZE > sz ? sz : BUFFSIZE, 0);
	}
	fclose(file);
}


unsigned __stdcall rec(void* ssock)
{
	int cc;
	SOCKET* sock = (SOCKET*)ssock;
	while (1)
	{
		cc = recv(*sock, buf, BUFFSIZE, 0);
		if (cc == SOCKET_ERROR || cc == 0)
		{
			printf("Error: %d.\n", GetLastError());     //出错。其后必须关闭套接字sock。
			break;
		}
		else if (cc > MAX_FILE_NAME) {
			FileStruct tem;
			memcpy(&tem, buf, sizeof(FileStruct));
			char file_name[MAX_FILE_NAME];
			strcpy(file_name, tem.fileName);
			add_str(file_name, file_name_head);
			long long sz = 0;
			if (!strcmp(tem.fileType, PDF) || !strcmp(tem.fileType, TXT)
			 || !strcmp(tem.fileType, JPG) || !strcmp(tem.fileType, PNG)
			 || !strcmp(tem.fileType, PPTX))
			{
				PDF_Struct size;
				memcpy(&size, buf + sizeof(FileStruct), sizeof(PDF_Struct));
				sz = size;
			}
			FILE* file;
			int num = 2;
			while (1)
			{
				file = fopen(file_name, "rb");
				if (file)
				{
					fclose(file);
					char app[10] = {};
					app_str(app, num);
					num++;
					char* pos;
					pos = strrchr(file_name, '(');
					if (!pos) pos = strchr(file_name, '.');
					strcpy(pos, app);
					strcat(file_name, ".");
					strcat(file_name, tem.fileType);
				}
				else {
					break;
				}
			}
			file = fopen(file_name, "wb");
			if (!file)
			{
				printf("FILE OPEN ERROR!\n");
				break;
			}
			while (sz)
			{
				cc = recv(*sock, buf, BUFFSIZE, 0);
				fwrite(buf, 1, BUFFSIZE, file);
				sz -= cc;
			}
			fclose(file);
		}
		else{
			buf[cc] = '\0';	                       // ensure null-termination				
			if (!strcmp(buf, QUIT)) break;
			printf("\r%s\n>", buf);                         // 显示所接收的字符串
		}
	}

	return 0;
}



int main(int argv, char** argc)
{
	WSADATA wsadata;
	WSAStartup(WSVERS, &wsadata);
	SOCKET	msock, ssock;
	msock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct  sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons((u_short)atoi(SERVICE));
	bind(msock, (struct sockaddr*) & sin, sizeof(sin));
	listen(msock, QUSIZE);

	printf("服务器已启动！\n\n");
	struct  sockaddr_in fsin;
	int hd_sz = sizeof(fsin);
	ssock = accept(msock, (struct sockaddr*) & fsin, &hd_sz);
	HANDLE thr = (HANDLE)_beginthreadex(NULL, 0, &rec, (void*)&ssock, 0, NULL);
	char comm[MAX_FILE_NAME];

	while (1)
	{
		printf(">");
		scanf_s("%[^\n]s", comm, MAX_FILE_NAME);
		getchar();
		if (!strncmp(comm, RDIR, 4)){
			strcpy(file_name_head, comm + 5);
		}
		else if (!strncmp(comm, CHAT, 4)){
			send(ssock, comm+5, strlen(comm)-5, 0);
		}
		else if(!strncmp(comm, SEND, 4)){
			send_file(ssock, comm + 5);
		}
		else if (!strcmp(comm, QUIT)){
			send(ssock, comm, strlen(comm), 0);
			break;
		}
		else{
			send(ssock, comm, strlen(comm), 0);
		}
	}

	CloseHandle(thr);
	closesocket(ssock);								// 关闭套接字
	closesocket(msock);								    // 关闭监听套接字
	WSACleanup();										// 卸载winsock library

	return 0;
}