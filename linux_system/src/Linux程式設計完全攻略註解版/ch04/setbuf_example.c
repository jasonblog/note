/* Example show usage of setbuf() &setvbuf() */
#include<stdio.h>
#include<error.h>
int main(int argc , char** argv)
{
    int i;
    FILE* fp;
    char msg1[] = "hello,wolrd\n";
    char msg2[] = "hello\nworld";
    char buf[128];                  //

    if ((fp = fopen("no_buf1.txt", "w")) == NULL) {
        perror("file open failure!");
        return (-1);
    }

    setbuf(fp, NULL);               //
    memset(buf, '\0', 128);
    fwrite(msg1 , 7 , 1 , fp);           //
    printf("test setbuf(no buf)!check no_buf1.txt\n");  //
    printf("now buf data is :buf=%s\n", buf);   //
    printf("press enter to continue!\n");
    getchar();
    fclose(fp);             //

    if ((fp = fopen("no_buf2.txt", "w")) == NULL) {
        perror("file open failure!");
        return (-1);
    }

    setvbuf(fp , NULL, _IONBF , 0);          //
    memset(buf, '\0', 128);
    fwrite(msg1 , 7, 1 , fp);            //
    printf("test setvbuf(no buf)!check no_buf2.txt\n ");
    printf("now buf data is :buf=%s\n", buf);   //
    printf("press enter to continue!\n");
    getchar();
    fclose(fp);                 //

    if ((fp = fopen("l_buf.txt", "w")) == NULL) {
        perror("file open failure!");
        return (-1);
    }

    setvbuf(fp , buf , _IOLBF , sizeof(buf));  //
    memset(buf, '\0', 128);
    fwrite(msg2 , sizeof(msg2) , 1 , fp);        //
    printf("test setvbuf(line buf)!check l_buf.txt, because line buf , only data before enter send to file\n");
    printf("now buf data is :buf=%s\n", buf);   //
    printf("press enter to continue!\n");
    getchar();
    fclose(fp);                 //

    if ((fp = fopen("f_buf.txt", "w")) == NULL) {
        perror("file open failure!");
        return (-1);
    }

    setvbuf(fp , buf , _IOFBF , sizeof(buf));
    memset(buf, '\0', 128);
    fwrite(msg2 , sizeof(msg2) , 1 , fp);
    printf("test setbuf(full buf)!check f_buf.txt\n");
    printf("now buf data is :buf=%s\n", buf);   //
    printf("press enter to continue!\n");
    getchar();
    fclose(fp);                 //
}


