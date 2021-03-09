#pragma warning( disable : 4996)
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#define USER_NAME_LEN 10
#define EMAIL_LEN 50
#define BUFF_LEN 10000
#define END "exit"
#define FILE_NAME "C:\\Users\\E480\\Desktop\\Persons.stru"

typedef struct {
    char username[USER_NAME_LEN];      // 员工名
    int level;                         // 工资级别
    char email[EMAIL_LEN];             // email地址
    time_t sendtime;//DWORD               // 发送时间
    time_t regtime;                    // 注册时间
}Person;

Person input_one_person()
{
    char buff[BUFF_LEN] = {};
    time_t	now;
    Person curr;
    printf("name:");
    scanf_s("%s", buff, BUFF_LEN);
    strcpy(curr.username, buff);
    if (!strcmp(curr.username, END)) return curr;
    printf("level:");
    scanf_s("%s", buff, BUFF_LEN);
    curr.level = atoi(buff);
    printf("email:");
    scanf_s("%s", buff, BUFF_LEN);
    strcpy(curr.email, buff);
    (void)time(&now);
    curr.regtime = now;
    curr.sendtime = now;
    return curr;
}

int main()
{
    //printf("%d", sizeof(Person));
    FILE* outFile = fopen(FILE_NAME, "wb");
    if (!outFile)
    {
        printf("ERROR!\n");
        return 0;
    }
    char buff[BUFF_LEN] = {};
    Person curr;
    while (1)
    {
        Person curr = input_one_person();
        if (!strcmp(curr.username, END)) break;
        memcpy(buff, (void*)&curr, sizeof(Person));
        if (fwrite(&curr, sizeof(Person), 1, outFile) != 1) {
            printf("file write error!\n");
        }
    }
    fclose(outFile);
    return 0;
}
