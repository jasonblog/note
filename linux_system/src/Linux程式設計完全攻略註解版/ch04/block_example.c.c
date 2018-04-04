#include<stdio.h>
int main(int argc, char* argv[])
{
    struct student {
        char name[10];
        int number;
    };
    FILE* fp = NULL;
    int i;
    struct student boya[2], boyb[2], *pp, *qq;

    if ((fp = fopen("aa.txt",
                    "w+")) == NULL) { //以可读写的方式打开文件；若该文件存在则清空，若不存在就创建
        //打开文件失败
        printf("can not open!\n");
        return -1;
    }

    pp = boya;
    qq = boyb;
    printf(“please input two students‘ name and number: \n");
           for (i=0;i<2;i++,pp++)
           scanf(" % s\ % d",pp->name,&pp->number);
           pp=boya;
           fwrite(pp,sizeof(struct student),2,fp); //将从键盘输入的信息写入到文件流fp中
           rewind(fp); //将读写位置定位到文件头
           fread(qq,sizeof(struct student),2,fp); //从文件流fp中读两个结构体到qq
           printf("name\t\t number\n");
           for(i=0;i<2;i++,qq++) //输出qq中的内容
           printf(" % s\t\t % d\n",qq->name,qq->number);
           fclose(fp);
           return 0;
       }
