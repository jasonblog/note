#ifndef _WAV_H_
#define _WAV_H_
typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned int U32;

#pragma pack(1)
struct RIFF_HEADER {
    U8 ID[4];   // 'R','I','F','F'
    U32 Size;
    U8 Type[4]; // 'W','A','V','E'
};
struct WAVE_FORMAT {
    U32 ID;
    U32 Size;
    U16 FormatTag;
    U16 Channels;
    U32 SamplesPerSec;
    U32 AvgBytesPerSec;
    U16 BlockAlign;
    U16 BitsPerSample;
    U16 pack; //∏Ωº”–≈œ¢
};


struct FACT_BLOCK {
    U8 szFactID[4]; // 'f','a','c','t'
    U32 dwFactSize;
    U32 wavFormat;
};
struct DATA_BLOCK {
    U8 ID[4]; // 'd','a','t','a'
    U32 DataSize;
};
#endif
