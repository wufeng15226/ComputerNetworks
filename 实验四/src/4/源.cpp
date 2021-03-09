#pragma warning( disable : 4996)
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define MAX_FILE_NAME 300
#define BUFF_LEN 100
#define MAX_FILE_NUM 10
#define END "exit"
typedef struct {
    char fileName[MAX_FILE_NAME];
    __int64 fileSize;
}FileStruct;

void add_str(char* str, char* front_app)
{
    int len = strlen(front_app);
    char tem[BUFF_LEN] = {};
    strcpy(tem, front_app);
    tem[len] = '\\';
    tem[len + 1] = '\0';
    strcpy(tem + len +1, str);
    strcpy(str, tem);
}

int stoi(char* str)
{
    int pos = 0;
    int rst = 0;
    while (str[pos])
    {
        if (str[pos] < '0' || str[pos]>'9') return 0;
        rst = rst * 10 + str[pos] - '0';
        pos++;
    }
    return rst;
}

void UnpackFile(char* folder,char* file)
{
    FILE* in_File = fopen(file, "rb");
    char buff[BUFF_LEN] = {};
    char name[MAX_FILE_NUM][BUFF_LEN] = {};
    int num = 0;
    FileStruct curr;
    while(fread(&curr, sizeof(FileStruct), 1, in_File))
    {
        int i = 0;
        for (; i < num; i++)
        {
            if (!strcmp(curr.fileName, name[i]))
            {
                char* app = NULL;
                char* tem = NULL;
                char* ttem = NULL;
                char dot[BUFF_LEN] = {'\0'};
                app = strrchr(curr.fileName, '.');
                if (app)
                {
                    strcpy(dot, app);
                }
                if ((tem = strrchr(curr.fileName, '('))&&(ttem = strrchr(curr.fileName, ')')))
                {
                    char times[BUFF_LEN] = {};
                    strncpy(times, tem+1, ttem - tem-1);
                    int n = stoi(times);
                    if (!n)
                    {
                        strcpy(app, "(2)");
                        strcpy(app + 3, dot);
                        break;
                    }
                    n++;
                    strcpy(tem + 1, itoa(n,times,10));
                    int len = strlen(times);
                    tem[len + 1] = ')';
                    if (app)
                    {
                        strcpy(tem + len + 2, dot);
                    }
                    //tem[len + 2 +strlen(dot)] = '\0';
                    break;
                }
                strcpy(app, "(2)");
                strcpy(app + 3, dot);
                break;
            }
        }
        if (i == num) strcpy(name[num++], curr.fileName);
        add_str(curr.fileName, folder);
        FILE* out_File = fopen(curr.fileName, "wb");
        int len = curr.fileSize;
        while (len)
        {
            if (len > BUFF_LEN)
            {
                fread(buff, BUFF_LEN, 1, in_File);
                fwrite(buff, BUFF_LEN, 1, out_File);
                len -= BUFF_LEN;
            }
            else
            {
                fread(buff, len, 1, in_File);
                fwrite(buff, len, 1, out_File);
                len = 0;
            }
        }
        fclose(out_File);
    }
    fclose(in_File);
}

int main()
{
    char buff[BUFF_LEN] = {};
    char tem[BUFF_LEN] = {};
    printf("输入目标文件夹：");
    scanf_s("%s", buff, BUFF_LEN);
    strcpy(tem, buff);
    printf("输入要解包的文件（含路径）：");
    scanf_s("%s", buff, BUFF_LEN);
    UnpackFile(tem, buff);
    return 0;
}