// pcm.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

//分离PCM16LE双声道音频采样数据的左声道和右声道
int pcm16_split(const char* url)
{
    FILE* fp = fopen(url, "rb+");
    FILE* fp1 = fopen("output_l.pcm", "wb+");
    FILE* fp2 = fopen("output_r.pcm", "wb+");
    unsigned char* sample = (unsigned char*)malloc(4);
    if (sample == NULL)
    {
        printf("malloc failed\n");
        return -1;
    }
    while (!feof(fp))
    {
        fread(sample, 1, 4, fp);
        //L
        fwrite(sample, 1, 2, fp1);
        //R
        fwrite(sample + 2, 1, 2, fp2);
    }
    free(sample);
    fclose(fp);
    fclose(fp1);
    fclose(fp2);
    return 0;
}

//将pcm16LE双声道音频采样数据中左声道的音量降一半
int pcm16_halfvolumeleft(const char* url)
{
    FILE* fp = fopen(url, "rb+");
    FILE* fp1 = fopen("output_halfleft.pcm", "wb+");
    int cnt = 0;
    unsigned char* sample = (unsigned char*)malloc(4);
    if (sample == NULL)
    {
        return -1;
    }
    while (!feof(fp))
    {
        short* samplenum = NULL;
        fread(sample, 1, 4, fp);
        samplenum = (short*)sample;
        *samplenum = *samplenum / 2;
        //L
        fwrite(sample, 1, 2, fp1);
        //R
        fwrite(sample + 2, 1, 2, fp1);
        cnt++;
    }
    printf("sample cnt:%d\n", cnt);
    free(sample);
    fclose(fp);
    fclose(fp1);
    return 0;
}

//将PCM16LE双声道音频采样数据的声音速度提高一倍
int pcm16_doublespeed(const char* url)
{
    FILE* fp = fopen(url, "rb+");
    FILE* fp1 = fopen("output_doublespeed.pcm", "wb+");
    int cnt = 0;
    unsigned char* sample = (unsigned char*)malloc(4);
    if (sample == NULL)
    {
        return -1;
    }
    
    while (!feof(fp))
    {
        fread(sample, 1, 4, fp);
        //采样寄点数的样值
        if (cnt % 2 != 0)
        {
            //L
            fwrite(sample, 1, 2, fp1);
            //R
            fwrite(sample + 2, 1, 2, fp1);
        }
        cnt++;
    }
    printf("sample cnt:%d\n", cnt);

    free(sample);
    fclose(fp);
    fclose(fp1);
    return 0;
}

//将PCM16LE双声道音频采样数据转换为PCM8音频采样数据
int pcm16_to_pcm8(const char* url)
{
    FILE* fp = fopen(url, "rb+");
    FILE* fp1 = fopen("output_8.pcm", "wb+");

    int cnt = 0;
    unsigned char* sample = (unsigned char*)malloc(4);
    if (sample == NULL)
    {
        return -1;
    }
    while (!feof(fp))
    {
        short* samplenum16 = NULL;
        char samplenum8 = 0;
        unsigned char samplenum8_u = 0;
        fread(sample, 1, 4, fp);
        //(-32768-32767)
        samplenum16 = (short*)sample;
        samplenum8 = (*samplenum16) >> 8;
        //(-128-127)
        samplenum8_u = samplenum8 + 128;
        //L
        fwrite(&samplenum8_u, 1, 1, fp1);
        
        samplenum16 = (short*)(sample + 2);
        samplenum8 = (*samplenum16) >> 8;
        samplenum8_u = samplenum8 + 128;
        //R
        fwrite(&samplenum8_u, 1, 1, fp1);
        cnt++;
    }

    printf("sample cnt:%d\n", cnt);

    free(sample);
    fclose(fp);
    fclose(fp1);
    return 0;
}

//将从PCM16LE单声道音频采样数据中截取一部分数据
int pcl16_cut_siglechannel(const char* url, int start_num, int dur_num)
{
    FILE* fp = fopen(url, "rb+");
    FILE* fp1 = fopen("output_cut.pcm", "wb+");
    FILE* fp_stat = fopen("output_cut.txt", "wb+");
    unsigned char* sample = (unsigned char*)malloc(2);
    if (sample == NULL)
    {
        return -1;
    }
    int cnt = 0;
    while (!feof(fp))
    {
        fread(sample, 1, 2, fp);
        if (cnt > start_num && cnt <= (start_num + dur_num))
        {
            fwrite(sample, 1, 2, fp1);

            short samplenum = sample[1];
            samplenum = samplenum * 256;
            samplenum = samplenum + sample[0];
            fprintf(fp_stat, "%6d,", samplenum);
            if (cnt % 10 == 0)
            {
                fprintf(fp_stat, "\n");
            }
        }
        cnt++;
    }
    free(sample);
    fclose(fp);
    fclose(fp1);
    fclose(fp_stat);
    return 0;
}

//将PCM16LE双声道音频采样数据转换为WAVE格式音频数据
int pcm16_to_wave(const char* pcmpath, int channels, int sample_rate, const char* wavepath)
{
    typedef struct WAVE_HEARDER
    {
        char fccID[4];
        unsigned long dwSize;
        char fccType[4];
    }WAVE_HEADER;
    typedef struct WAVE_FMT
    {
        char fccID[4];
        unsigned long dwSize;
        unsigned short wFormatTag;
        unsigned short wChannels;
        unsigned long dwSamplesPerSec;
        unsigned long dwAvgBytesPerSec;
        unsigned short wBlockAlign;
        unsigned short uiBitsPerSample;
    }WAVE_FMT;
    typedef struct WAVE_DATA
    {
        char fccID[4];
        unsigned long dwSize;
    }WAVE_DATA;

    if (channels == 0 || sample_rate == 0)
    {
        channels = 2;
        sample_rate = 44100;
    }
    int bits = 16;
    WAVE_HEADER pcmHeader;
    WAVE_FMT pcmFmt;
    WAVE_DATA pcmData;

    unsigned short m_pcmData;
    FILE* fp, * fpout;
    fp = fopen(pcmpath, "rb");
    fpout = fopen(wavepath, "wb+");

    //WAVE_HEADER
    memcpy(pcmHeader.fccID, "RIFF", strlen("RIFF"));
    memcpy(pcmHeader.fccType, "WAVE", strlen("WAVE"));
    fseek(fpout, sizeof(WAVE_HEADER), 1);
    //WAVE_FMT
    pcmFmt.dwSamplesPerSec = sample_rate;
    pcmFmt.dwAvgBytesPerSec = pcmFmt.dwSamplesPerSec * sizeof(m_pcmData);
    pcmFmt.uiBitsPerSample = bits;
    memcpy(pcmFmt.fccID, "fmt", strlen("fmt "));
    pcmFmt.dwSize = 16;
    pcmFmt.wBlockAlign = 2;
    pcmFmt.wChannels = channels;
    pcmFmt.wFormatTag = 1;
    fwrite(&pcmFmt, sizeof(WAVE_FMT), 1, fpout);
    //WAVE_DATA
    memcpy(pcmData.fccID, "data", strlen("data"));
    pcmData.dwSize = 0;
    fseek(fpout, sizeof(WAVE_DATA), SEEK_CUR);
    fread(&m_pcmData, sizeof(unsigned short), 1, fp);
    while (!feof(fp))
    {
        pcmData.dwSize += 2;
        fwrite(&m_pcmData, sizeof(unsigned short), 1, fpout);
        fread(&m_pcmData, sizeof(unsigned short), 1, fp);
    }
    pcmHeader.dwSize = 44 + pcmData.dwSize;
    rewind(fpout); // 将文件指针指向文件开头
    fwrite(&pcmHeader, sizeof(WAVE_HEADER), 1, fpout);
    fseek(fpout, sizeof(WAVE_FMT), SEEK_CUR);
    fwrite(&pcmData, sizeof(WAVE_DATA), 1, fpout);

    return 0;
}

int main()
{
    //std::cout << "Hello World!\n";
    //pcm16_split("NocturneNo2inEflat_44.1k_s16le.pcm");
    //pcm16_halfvolumeleft("NocturneNo2inEflat_44.1k_s16le.pcm");
    //pcm16_doublespeed("NocturneNo2inEflat_44.1k_s16le.pcm");
    //pcm16_to_pcm8("NocturneNo2inEflat_44.1k_s16le.pcm");
    //pcl16_cut_siglechannel("drum.pcm", 2360, 120);
    pcm16_to_wave("NocturneNo2inEflat_44.1k_s16le.pcm", 2, 44100, "output_nocturne.wav");
    return 0;
}
