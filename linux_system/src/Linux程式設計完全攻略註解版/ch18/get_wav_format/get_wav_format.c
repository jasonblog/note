#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"wav.h"
int main(int argc, char* argv[])
{
    struct RIFF_HEADER myriff;
    struct WAVE_FORMAT mywav;
    struct FACT_BLOCK myfact;

    if (argc < 2) {
        printf("usage %s wav_format_file\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    FILE* fp = NULL;

    if (NULL == (fp = fopen(argv[1], "r"))) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    fread(&myriff, sizeof(myriff), 1, fp);
    fread(&mywav, sizeof(mywav), 1, fp);

    printf("file size=%u Bytes\n", myriff.Size + 8);
    printf("channel:%u\n", mywav.Channels);
    printf("dwSamplesPerSec:%u\n", mywav.SamplesPerSec);
    printf("size:%u\n", mywav.BitsPerSample);

}

