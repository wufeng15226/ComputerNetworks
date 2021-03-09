#pragma warning( disable : 4996)
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#define USER_NAME_LEN 10
#define EMAIL_LEN 50
#define BUFF_LEN 10000
#define TIME_BUF_LEN 30
#define END "exit"
#define FILE_NAME "C:\\Users\\E480\\Desktop\\Persons.stru"

typedef struct {
    char username[USER_NAME_LEN];      // Ա����
    int level;                         // ���ʼ���
    char email[EMAIL_LEN];             // email��ַ
    time_t sendtime;//DWORD               // ����ʱ��
    time_t regtime;                    // ע��ʱ��
}Person;

int main()
{
    //printf("%d", sizeof(Person));
    FILE* inFile = fopen(FILE_NAME, "rb");
    if (!inFile)
    {
        printf("ERROR!\n");
        return 0;
    }
    char buff[BUFF_LEN] = {};
    char sendtime[TIME_BUF_LEN] = {};
    char regtime[TIME_BUF_LEN] = {};

    Person curr;
    while (fread(buff, sizeof(Person), 1, inFile))
    {
        memcpy((void*)&curr, buff, sizeof(Person));
        ctime_s(sendtime, TIME_BUF_LEN, &curr.sendtime);
        ctime_s(regtime, TIME_BUF_LEN, &curr.regtime);
        printf("������%s  ����%d  �����ʼ���%s  ����ʱ�䣺%s\n ע��ʱ�䣺%s\n"
            , curr.username, curr.level, curr.email, sendtime, regtime);
    }
    fclose(inFile);
    return 0;
}
