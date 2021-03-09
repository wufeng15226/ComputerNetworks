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
    char username[USER_NAME_LEN];      // 员工名
    int level;                         // 工资级别
    char email[EMAIL_LEN];             // email地址
    time_t sendtime;//DWORD               // 发送时间
    time_t regtime;                    // 注册时间
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
        printf("姓名：%s  级别：%d  电子邮件：%s  发送时间：%s\n 注册时间：%s\n"
            , curr.username, curr.level, curr.email, sendtime, regtime);
    }
    fclose(inFile);
    return 0;
}
