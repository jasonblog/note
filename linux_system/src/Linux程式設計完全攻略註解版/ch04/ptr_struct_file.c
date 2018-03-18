#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define prt(CONTENT,MSG) printf(CONTENT":\t%p\n",MSG)

int main(int argc, char* argv[])
{
    FILE* fp_src, *fp_des;
    char buffer[10], buffer1[128];
    int i;

    if ((fp_src = fopen(argv[1], "r+")) == NULL) { //
        perror("open1");
        exit(EXIT_FAILURE);
    }

    if ((fp_des = fopen(argv[2], "w+")) == NULL) { //
        perror("open2");
        exit(EXIT_FAILURE);
    }

    setvbuf(fp_src, buffer1, _IOLBF, 128); //

    do {
        prt("src_IO_read_ptr", fp_src->_IO_read_ptr);   //
        prt("_IO_read_end", fp_src->_IO_read_end);
        prt("_IO_read_base", fp_src->_IO_read_base);
        prt("src_IO_write_ptr", fp_src->_IO_write_ptr); //
        prt("_IO_write_base", fp_src->_IO_write_base);
        prt("_IO_write_end", fp_src->_IO_write_end);

        prt("_IO_buf_base\t", fp_src->_IO_buf_base); //
        prt("_IO_buf_end\t", fp_src->_IO_buf_end);

        memset(buffer, '\0', 10);
        i = fread(buffer, 1, 10, fp_src);       //read
        fwrite(buffer, 1, i, fp_des);       //write

        prt("des_IO_read_ptr", fp_des->_IO_read_ptr);   //
        prt("des_IO_write_ptr", fp_des->_IO_write_ptr); //
    } while (i == 10);

    fclose(fp_src);
    fclose(fp_des);
}


