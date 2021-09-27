// h264Parser.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

typedef enum
{
    NALU_TYPE_SLICE = 1,
    NALU_TYPE_DPA,
    NALU_TYPE_DPB,
    NALU_TYPE_DPC,
    NALU_TYPE_IDR,
    NALU_TYPE_SEI,
    NALU_TYPE_SPS,
    NALU_TYPE_PPS,
    NALU_TYPE_AUD,
    NALU_TYPE_EOSEQ,
    NALU_TYPE_EOSTREAM,
    NALU_TYPE_FILL
}NALU_TYPE;

typedef enum
{
    NALU_PRIORITY_DISPOSABLE = 0,
    NALU_PRIRITY_LOW = 1,
    NALU_PRIORITY_HIGH = 2,
    NALU_PRIORITY_HIGHEST = 3
}NALU_PRIORITY;


typedef struct
{
    int startCodePrefixLen; //起始码长度，一帧的开始为4，其他为3
    unsigned len;           //Length of the NAL unit,(Excluding the start code, which does not belong to the NALU)
    unsigned maxSize;       //NAL unit Buffer Size
    int forbiddenBit;       //should be always FALSE
    int nalReferenceIdc;    //NALU_PRIORITY_xxxx
    int nalUnitType;        //NALU_TYPE_xxxx
    char* buf;              //contains the first byte followed by the EBSP
}NALU_T;

FILE* h264BitStream = NULL; //the bit stream file
int info3 = 0, info4 = 0;

static int findStartCode3(unsigned char* buf)
{
    //0x000001
    if (buf[0] != 0 || buf[1] != 0 ||  buf[2] != 1)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

static int findStartCode4(unsigned char* buf)
{
    //0x00000001
    if (buf[0] != 0 || buf[1] != 0 || buf[2] != 0 || buf[3] != 1)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int getAnnexbNALU(NALU_T* nalu)
{
    int pos = 0;
    int startCodeFound, reWind;
    unsigned char* buf;
    if ((buf = (unsigned char*)calloc(nalu->maxSize, sizeof(char))) == NULL)
    {
        printf("GetAnnexbNALU: Could not allocate Buf memory.\n");
    }
    //先读3个判断后再读一个
    nalu->startCodePrefixLen = 3;
    if (3 != fread(buf, 1, 3, h264BitStream))
    {
        free(buf);
        return 0;
    }
    
    info3 = findStartCode3(buf);
    if (info3 != 1)
    {
        if (1 != fread(buf + 3, 1, 1, h264BitStream))
        {
            free(buf);
            return 0;
        }
        info4 = findStartCode4(buf);
        if (info4 != 1)
        {
            free(buf);
            return -1;
        }
        else
        {
            pos = 4;
            nalu->startCodePrefixLen = 4;
        }
    }
    else
    {
        pos = 3;
        nalu->startCodePrefixLen = 3;
    }

    startCodeFound = 0;
    info3 = 0;
    info4 = 0;

    while (!startCodeFound)
    {
        //读取到了文件末尾
        if (feof(h264BitStream))
        {
            nalu->len = (pos - 1) - nalu->startCodePrefixLen;
            memcpy(nalu->buf, &buf[nalu->startCodePrefixLen], nalu->len);
            nalu->forbiddenBit = nalu->buf[0] & 0x80; //1bit
            nalu->nalReferenceIdc = nalu->buf[0] & 0x60;//2 bit
            nalu->nalUnitType = (nalu->buf[0]) & 0x1f;//5bif
            free(buf);
            return pos - 1;
        }
        buf[pos++] = fgetc(h264BitStream);
        info4 = findStartCode4(&buf[pos - 4]);
        if (info4 != 1)
        {
            info3 = findStartCode3(&buf[pos - 3]);
        }
        startCodeFound = (info3 == 1 || info4 == 1);
    }

    //发现了下一个startcode,重新定位至startcode处
    reWind = (info4 == 1) ? -4 : -3;
    if (0 != fseek(h264BitStream, reWind, SEEK_CUR))
    {
        free(buf);
        printf("GetAnnexbNALU: Cannot fseek in the bit stream file");
        return -1;
    }

    //定位到startcode处，并减去上一次startcode的长度
    nalu->len = (pos + reWind) - nalu->startCodePrefixLen;
    memcpy(nalu->buf, &buf[nalu->startCodePrefixLen], nalu->len);
    nalu->forbiddenBit = nalu->buf[0] & 0x80; //1bit
    nalu->nalReferenceIdc = nalu->buf[0] & 0x60;//2 bit
    nalu->nalUnitType = (nalu->buf[0]) & 0x1f;//5bif
    free(buf);

    return (pos + reWind);
}

int h264_parser(const char * url)
{
    NALU_T* n = NULL;
    int bufferSize = 100000;
    FILE* myout = stdout;
    h264BitStream = fopen(url, "rb+");
    if (h264BitStream == NULL)
    {
        printf("open file failed.\n");
        return 0;
    }

    n = (NALU_T*)calloc(1, sizeof(NALU_T));
    if (n == NULL)
    {
        printf("alloc NALU failed.\n");
        return 0;
    }

    n->maxSize = bufferSize;
    n->buf = (char*)calloc(bufferSize, sizeof(char));
    if (n->buf == NULL)
    {
        free(n);
        n = NULL;
        printf("alloc NALU:n->buf failed.\n");
        return 0;
    }

    int dataOffset = 0;
    int nalNum = 0;
    printf("-----+-------- NALU Table ------+---------+\n");
    printf(" NUM |    POS  |    IDC |  TYPE |   LEN   |\n");
    printf("-----+---------+--------+-------+---------+\n");

    //解析h264文件
    while (!feof(h264BitStream))
    {
        int dataLength = getAnnexbNALU(n);
        if (dataLength <= 0)
        {
            return 0;
        }
        char typeStr[20] = { 0 };
        switch (n->nalUnitType)
        {
        case NALU_TYPE_SLICE    : sprintf(typeStr, "SLICE"); break;
        case NALU_TYPE_DPA      : sprintf(typeStr, "DPA"); break;
        case NALU_TYPE_DPC      : sprintf(typeStr, "DPC"); break;
        case NALU_TYPE_IDR      : sprintf(typeStr, "IDR"); break;
        case NALU_TYPE_SEI      : sprintf(typeStr, "SEI"); break;
        case NALU_TYPE_SPS      : sprintf(typeStr, "SPS"); break;
        case NALU_TYPE_PPS      : sprintf(typeStr, "PPS"); break;
        case NALU_TYPE_AUD      : sprintf(typeStr, "AUD"); break;
        case NALU_TYPE_EOSEQ    : sprintf(typeStr, "EOSEQ"); break;
        case NALU_TYPE_EOSTREAM : sprintf(typeStr, "EOSTREAM"); break;
        case NALU_TYPE_FILL     : sprintf(typeStr, "FILL"); break;
        }
        char idcStr[20] = { 0 };
        switch (n->nalReferenceIdc)
        {
        case NALU_PRIORITY_DISPOSABLE : sprintf(idcStr, "DISPOS"); break;
        case NALU_PRIRITY_LOW         : sprintf(idcStr, "LOW"); break;
        case NALU_PRIORITY_HIGH       : sprintf(idcStr, "HIGH"); break;
        case NALU_PRIORITY_HIGHEST    : sprintf(idcStr, "HIGHEST"); break;
        }

        fprintf(myout, "%5d| %8d| %7s| %6s| %8d|\n", nalNum, dataOffset, idcStr, typeStr, n->len);
        dataOffset = dataOffset + dataLength;
        nalNum++;
    }

    //free
    if (n)
    {
        if (n->buf)
        {
            free(n->buf);
            n->buf = NULL;
        }
        free(n);
        n = NULL;
    }

    return 0;
}


int main()
{
    h264_parser("sintel.h264");
    //std::cout << "Hello World!\n";
}