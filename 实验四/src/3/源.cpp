#pragma warning( disable : 4996)
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<Windows.h>
#define MAX_FILE_NAME 300
#define BUFF_LEN 100
#define MAX_FILE_NUM 10
#define END "exit"
typedef struct{
    char fileName[MAX_FILE_NAME];
    __int64 fileSize;
}FileStruct;

long getFileSize(char* fileName)
{
    long rst = 0;
    char buff[BUFF_LEN] = {};
    FILE* in_File = fopen(fileName, "rb");
    if (!in_File)
    {
        printf("IN FILE OPEN ERROR!");
        return 0;
    }
    while (fread(buff, 1, 1, in_File)) {
        rst ++;
    }
    fclose(in_File);
    return rst;
}



void packFile(char* out_File_name, FileStruct* file, int num)
{
    FILE* out_File = fopen(out_File_name, "wb");
    if (!out_File)
    {
        printf("OUT FILE OPEN ERROR!");
        return;
    }
    for (int i = 0; i < num; i++)
    {
        FILE* in_File = fopen(file[i].fileName, "rb");
        if (!in_File)
        {
            printf("IN FILE OPEN ERROR!");
            return;
        }
        char buff[BUFF_LEN] = {};
        int len = 0;
        strcpy(buff, strrchr(file[i].fileName, '\\') + 1);
        strcpy(file[i].fileName, buff);
        fwrite((void*)&file[i], sizeof(FileStruct), 1, out_File);
        while ((len = fread(buff, 1, BUFF_LEN, in_File)) >= BUFF_LEN) {
            fwrite(buff, 1, BUFF_LEN, out_File);
        }
        fwrite(buff, 1, len, out_File);   // 写入剩余部分
        fclose(in_File);
    }
    fclose(out_File);
}

int main()
{
    char buff[BUFF_LEN] = {};
    char tem[BUFF_LEN] = {};
    int num = 0;
    FileStruct file[MAX_FILE_NUM] = {};
    printf("输入目标文件名（含路径）：");
    scanf_s("%s", buff, BUFF_LEN);
    strcpy(tem, buff);
    while (1)
    {
        printf("输入要打包的#%d文件（含路径）：",num+1);
        scanf_s("%s", buff, BUFF_LEN);
        if (!strcmp(buff, END)) break;
        strcpy(file[num].fileName, buff);
        file[num].fileSize = getFileSize(buff);
        num++;
    }
    packFile(tem, file, num);
    return 0;
}