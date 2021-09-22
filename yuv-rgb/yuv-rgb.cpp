// yuv-rgb.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。


#include <iostream>

/**
 * 分离yuv420文件中的y、u、v分量
 * @param url  文件  
 * @param w 宽
 * @param h 高
 * @param num 帧数量
 * 
 */
int yuv420_split(const char* url, int w, int h, int num)
{
    FILE* fp = fopen(url, "rb+");
    FILE* fp1 = fopen("output_420_y.y", "wb+");
    FILE* fp2 = fopen("output_420_u.y", "wb+");
    FILE* fp3 = fopen("output_420_v.y", "wb+");

    //y存储w*h，u存储w*h/4，v存储w*h/4
    unsigned char* pic = (unsigned char*)malloc(w * h * 3 / 2);
    if (pic == NULL)
    {
        printf("malloc failed.\n");
        return -1;
    }

    for (int i = 0; i < num; i++)
    {
        fread(pic, 1, w * h * 3 / 2, fp);
        //Y
        fwrite(pic, 1, w * h, fp1);
        //U
        fwrite(pic + w * h, 1, w * h / 4, fp2);
        //V
        fwrite(pic + w * h * 5 / 4, 1, w * h / 4, fp3);
    }

    free(pic);
    fclose(fp);
    fclose(fp1);
    fclose(fp2);
    fclose(fp3);

    printf("success.\n");
    return 0;
}

/**
 * 分离yuv440文件中的y、u、v分量
 * @param url  文件
 * @param w 宽
 * @param h 高
 * @param num 帧数量
 *
 */
int yuv444_split(const char* url, int w, int h, int num)
{
    FILE* fp = fopen(url, "rb+");
    FILE* fp1 = fopen("output_444_y.y", "wb+");
    FILE* fp2 = fopen("output_444_u.y", "wb+");
    FILE* fp3 = fopen("output_444_v.y", "wb+");

    //y存储w*h，u存储w*h，v存储w*h
    unsigned char* pic = (unsigned char*)malloc(w * h * 3);
    if (pic == NULL)
    {
        printf("malloc failed.\n");
        return -1;
    }

    for (int i = 0; i < num; i++)
    {
        fread(pic, 1, w * h * 3, fp);
        //Y
        fwrite(pic, 1, w * h, fp1);
        //u
        fwrite(pic + w * h, 1, w * h, fp2);
        //v
        fwrite(pic + w * h * 2, 1, w * h, fp3);
    }

    free(pic);
    fclose(fp);
    fclose(fp1);
    fclose(fp2);
    fclose(fp3);

    printf("success.\n");
    return 0;
}


/**
 * 去掉yuv420p格式像素数据的彩色，变成纯粹的灰度图
 * @param url  文件
 * @param w 宽
 * @param h 高
 * @param num 帧数量
 *
 */
int yuv420_gray(const char* url, int w, int h, int num)
{
    FILE* fp = fopen(url, "rb+");
    FILE* fp1 = fopen("output_yuv420_gray.yuv", "wb+");
    unsigned char* pic = (unsigned char*)malloc(w * h * 3 / 2);
    if (pic == NULL)
    {
        printf("malloc failed.\n");
        return -1;
    }

    for (int i = 0; i < num; i++)
    {
        fread(pic, 1, w * h * 3 / 2, fp);

        //gray 将yuv格式数据变成灰度图像，只需将U、V分量设置成128即可。
        //因为U、V是图像中的经过偏置处理的色度分量。色度分量在偏置处理前的取值范围是-128~127，这时候无色对应的
        //是0值。经过偏置后色度分量取值变成了0~155，因而此时的无色对应的就是128
        memset(pic + w * h, 128, w * h / 2);
        fwrite(pic, 1, w * h * 3 / 2, fp1);
    }

    free(pic);
    fclose(fp);
    fclose(fp1);

    printf("success\n");
    return 0;
}

/**
 * 将yuv420p像素数据的亮度减半
 * @param url  文件
 * @param w 宽
 * @param h 高
 * @param num 帧数量
 *
 */
int yuv420p_halfy(const char* url, int w, int h, int num)
{
    FILE* fp = fopen(url, "rb+");
    FILE* fp1 = fopen("output_yuv420_falfy.yuv", "wb+");
    unsigned char* pic = (unsigned char*)malloc(w * h * 3 / 2);
    if (pic == NULL)
    {
        printf("malloc failed.\n");
        return -1;
    }

    for (int i = 0; i < num; i++)
    {
        //half
        fread(pic, 1, w * h * 3 / 2, fp);

        for (int j = 0; j < w * h; j++)
        {
            unsigned char temp = pic[j] / 2;
            pic[j] = temp;
        }
        fwrite(pic, 1, w * h * 3 / 2, fp1);
    }

    free(pic);
    fclose(fp);
    fclose(fp1);

    printf("success.\n");
    return 0;
}

//将yuv420p像素数据的周围加上边框
//将大于border的周围设置为最大亮度
int yuv420_border(const char* url, int w, int h, int border, int num)
{
    FILE* fp = fopen(url, "rb+");
    FILE* fp1 = fopen("output_border.yuv", "wb+");
    unsigned char* pic = (unsigned char*)malloc(w * h * 3 / 2);
    if (pic == NULL)
    {
        printf("malloc failed.\n");
        return -1;
    }
    for (int i = 0; i < num; i++)
    {
        fread(pic, 1, w * h * 3 / 2, fp);
        // v
        for (int j = 0; j < h; j++)
        {
            for (int k = 0; k < w; k++)
            {
                if (k < border || k >(w - border) || j < border || j > (h - border))
                {
                    pic[j * w + k] = 255;
                }
            }
        }
        fwrite(pic, 1, w * h * 3 / 2, fp1);
    }
    free(pic);
    fclose(fp);
    fclose(fp1);
    return 0;
}

//生成yuv420p格式的灰阶测试图
//ymax:最大亮度值 ymin:最小亮度值 barnum:灰阶数量
int yuv420_garybar(int width, int height, int ymin, int ymax, int barnum, const char* url_out)
{
    int barwidth;
    float lum_inc;
    unsigned char lum_temp;
    int uv_width, uv_height;
    FILE* fp = NULL;
    unsigned char* data_y = NULL;
    unsigned char* data_u = NULL;
    unsigned char* data_v = NULL;
    int t = 0, i = 0, j = 0;

    barwidth = width / barnum;//每一个灰阶的宽度
    lum_inc = ((float)(ymax - ymin)) / ((float)(barnum - 1)); //每个灰阶格子的灰度
    uv_width = width / 2;
    uv_height = height / 2;
    data_y = (unsigned char*)malloc(width * height);
    data_u = (unsigned char*)malloc(uv_width * uv_height);
    data_v = (unsigned char*)malloc(uv_width * uv_height);

    if ((fp = fopen(url_out, "wb+")) == NULL)
    {
        printf("cannot create file\n");
        return -1;
    }
    printf("Y, U, V value from picture's left to right:\n");
    for (t = 0; t < width / barwidth; t++)
    {
        lum_temp = ymin + (char)(t * lum_inc);
        printf("%3d, 128, 128\n", lum_temp);
    }
    //gen data
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            t = i / barwidth;
            lum_temp = ymin + (char)(t * lum_inc);
            data_y[j * width + i] = lum_temp;
        }
    }
    for (j = 0; j < uv_height; j++)
    {
        for (i = 0; i < uv_width; i++)
        {
            data_u[j * uv_width + i] = 128;
        }
    }
    for (j = 0; j < uv_height; j++)
    {
        for (i = 0; i < uv_width; i++)
        {
            data_v[j * uv_width + i] = 128;
        }
    }

    fwrite(data_y, width * height, 1, fp);
    fwrite(data_u, uv_width * uv_height, 1, fp);
    fwrite(data_v, uv_width * uv_height, 1, fp);
}

int yuv420_psnr(const char* url1, const char* url2, int w, int h, int num)
{
    FILE* fp1 = fopen(url1, "rb+");
    FILE* fp2 = fopen(url2, "rb+");
    unsigned char* pic1 = (unsigned char*)malloc(w * h);
    unsigned char* pic2 = (unsigned char*)malloc(w * h);
    if (pic1 == NULL || pic2 == NULL)
    {
        printf("malloc failed.\n");
        return -1;
    }
    for (int i = 0; i < num; i++)
    {
        fread(pic1, 1, w * h, fp1);
        fread(pic2, 1, w * h, fp2);
        double mse_sum = 0, mse = 0, psnr = 0;
        for (int j = 0; j < w * h; j++)
        {
            mse_sum += pow((double)(pic1[j] - pic2[j]), 2);
        }
        mse = mse_sum / (w * h);
        psnr = 10 * log10(255.0 * 255.0 / mse);
        printf("%5.3f\n", psnr);
        fseek(fp1, w * h / 2, SEEK_CUR);
        fseek(fp2, w * h / 2, SEEK_CUR);
    }
    free(pic1);
    free(pic2);
    fclose(fp1);
    fclose(fp2);
    return 0;
}

//分离RGB24像素数据中的R、G、B分量
int rgb24_split(const char* url, int w, int h, int num)
{
    FILE* fp = fopen(url, "rb+");
    FILE* fp1 = fopen("output_r.y", "wb+");
    FILE* fp2 = fopen("output_g.y", "wb+");
    FILE* fp3 = fopen("output_b.y", "wb+");
    unsigned char* pic = (unsigned char*)malloc(w * h * 3);
    if (pic == NULL)
    {
        printf("malloc failed.\n");
        return -1;
    }
    for (int i = 0; i < num; i++)
    {
        fread(pic, 1, w * h * 3, fp);
        for (int j = 0; j < w * h * 3; j = j + 3)
        {
            //R
            fwrite(pic + j, 1, 1, fp1);
            //G
            fwrite(pic + j + 1, 1, 1, fp2);
            //B
            fwrite(pic + j + 2, 1, 1, fp3);
        }
    }
    free(pic);
    fclose(fp);
    fclose(fp1);
    fclose(fp2);
    fclose(fp3);
    return 0;
}

//将RGB24格式像素数据封装为BMP图像
int rgb24_to_bmp(const char* rgb24path, int width, int height, const char* bmppath)
{
    typedef struct
    {
        long imageSize;
        long blank;
        long startPosition;
    }BmpHead;

    typedef struct
    {
        long length;//该结构大小
        long width;
        long  height;
        unsigned short colorPlane;//目标设备的级别，必须为1
        unsigned short bitColor;
        long zipFormat;
        long realSize;
        long xPels;
        long yPels;
        long colorUse;
        long colorImportant;
    }InfoHead;

    int i = 0, j = 0;
    BmpHead m_BMPHeader = { 0 };
    InfoHead m_BMPInfoHeader = { 0 };
    char bfTYpe[2] = { 'B', 'M' };
    int header_size = sizeof(bfTYpe) + sizeof(BmpHead) + sizeof(InfoHead);
    unsigned char* rgb24_buffer = NULL;
    FILE* fp_rgb24 = NULL, * fp_bmp = NULL;
    if ((fp_rgb24 = fopen(rgb24path, "rb")) == NULL)
    {
        printf("Error: Cannot open input RGB24 file.\n");
        return -1;
    }
    if ((fp_bmp = fopen(bmppath, "wb")) == NULL)
    {
        printf("Error: Cannot open output BMP file.\n");
        return -1;
    }
    rgb24_buffer = (unsigned char*)malloc(width * height * 3);
    if (rgb24_buffer == NULL)
    {
        printf("malloc failed.\n");
        return -1;
    }
    fread(rgb24_buffer, 1, width* height * 3, fp_rgb24);

    m_BMPHeader.imageSize = 3 * width * height + header_size;
    m_BMPHeader.startPosition = header_size;
    
    m_BMPInfoHeader.length = sizeof(InfoHead);
    m_BMPInfoHeader.width = width;
    //BMP storage pixel data in opposite direction of Y-axis (from bottom to top).
    m_BMPInfoHeader.height = -height;
    m_BMPInfoHeader.colorPlane = 1;
    m_BMPInfoHeader.bitColor = 24;
    m_BMPInfoHeader.realSize = 3 * width * height;

    fwrite(bfTYpe, 1, sizeof(bfTYpe), fp_bmp);
    fwrite(&m_BMPHeader, 1, sizeof(m_BMPHeader), fp_bmp);
    fwrite(&m_BMPInfoHeader, 1, sizeof(m_BMPInfoHeader), fp_bmp);

    //BMP save R1|G1|B1,R2|G2|B2 as B1|G1|R1,B2|G2|R2
    //It saves pixel data in Little Endian
    //So we change 'R' and 'B'
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            unsigned char temp = rgb24_buffer[(j * width + i) * 3 + 2];
            rgb24_buffer[(j * width + i) * 3 + 2] = rgb24_buffer[(j * width + i) * 3 + 0];
            rgb24_buffer[(j * width + i) * 3 + 0] = temp;
        }
    }
    fwrite(rgb24_buffer, 3 * width * height, 1, fp_bmp);
    fclose(fp_bmp);
    fclose(fp_rgb24);
    free(rgb24_buffer);
    printf("Finish generate %s!\n", bmppath);
    return 0;
}

//将rgb24格式数据转换为yuv420格式像素数据
unsigned char clip_value(unsigned char x, unsigned char min_val, unsigned char max_val)
{
    if (x > max_val)
    {
        return max_val;
    }
    else if (x < min_val)
    {
        return min_val;
    }
    else
    {
        return x;
    }
}

void RGB24_TO_YUV420(unsigned char* rgb_buf, int w, int h, unsigned char* yuv_buf)
{
    unsigned char* ptrY, * ptrU, * ptrV, * ptrRGB;
    memset(yuv_buf, 0x0, w * h * 3 / 2);
    ptrY = yuv_buf;
    ptrU = yuv_buf + w * h;
    ptrV = ptrU + w * h * 1 / 4;
    unsigned char y, u, v, r, g, b;
    
    for (int j = 0; j < h; j++)
    {
        ptrRGB = rgb_buf + w * j * 3;
        for (int i = 0; i < w; i++)
        {
            r = *(ptrRGB++);
            g = *(ptrRGB++);
            b = *(ptrRGB++);
            y = (unsigned char)((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;
            u = (unsigned char)((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128;
            v = (unsigned char)((112 * r - 94 * g - 18 * b + 128) >> 8) + 128;
            *(ptrY++) = clip_value(y, 0, 255);
            if (j % 2 == 0 && i % 2 == 0)
            {
                *(ptrU++) = clip_value(u, 0, 255);
            }
            else
            {
                if (i % 2 == 0)
                {
                    *(ptrV++) = clip_value(v, 0, 255);
                }
            }
        }
    }
    return;
}

int rgb24_to_yuv420(const char* url_in, int w, int h, int num, const char* url_out)
{
    FILE* fp = fopen(url_in, "rb+");
    FILE* fp1 = fopen(url_out, "wb+");
    unsigned char* pic_rgb24 = (unsigned char*)malloc(w * h * 3);
    unsigned char* pic_yuv420 = (unsigned char*)malloc(w * h * 3 / 2);
    if (pic_rgb24 == NULL || pic_yuv420 == NULL)
    {
        printf("malloc failed\.n");
        return -1;
    }
    for (int i = 0; i < num; i++)
    {
        fread(pic_rgb24, 1, w * h * 3, fp);
        RGB24_TO_YUV420(pic_rgb24, w, h, pic_yuv420);
        fwrite(pic_yuv420, 1, w * h * 3 / 2, fp1);
    }

    free(pic_rgb24);
    free(pic_yuv420);
    fclose(fp);
    fclose(fp1);
    return 0;
}

//生成rgb24格式的彩条测试图
int rgb24_colorbar(int width, int heigth, const char* url_out)
{
    unsigned char* data = NULL;
    int barwidth;
    char filename[100] = {0};
    FILE* fp = NULL;
    int i = 0, j = 0;

    data = (unsigned char*)malloc(width * heigth * 3);
    if (data == NULL)
    {
        printf("malloc failed.\n");
        return -1;
    }
    barwidth = width / 8;
    if ((fp = fopen(url_out, "wb+")) == NULL)
    {
        printf("Error:Cannot create file.\n");
        return -2;
    }
    for (j = 0; j < heigth; j++)
    {
        for (i = 0; i < width; i++)
        {
            int barnum = i / barwidth;
            switch (barnum)
            {
            case 0:
            {
                data[(j * width + i) * 3 + 0] = 255;
                data[(j * width + i) * 3 + 1] = 255;
                data[(j * width + i) * 3 + 2] = 255;
                break;
            }
            case 1:
            {
                data[(j * width + i) * 3 + 0] = 255;
                data[(j * width + i) * 3 + 1] = 255;
                data[(j * width + i) * 3 + 2] = 0;
                break;
            }
            case 2:
            {
                data[(j * width + i) * 3 + 0] = 0;
                data[(j * width + i) * 3 + 1] = 255;
                data[(j * width + i) * 3 + 2] = 255;
                break;
            }
            case 3:
            {
                data[(j * width + i) * 3 + 0] = 0;
                data[(j * width + i) * 3 + 1] = 255;
                data[(j * width + i) * 3 + 2] = 0;
                break;
            }
            case 4:
            {
                data[(j * width + i) * 3 + 0] = 255;
                data[(j * width + i) * 3 + 1] = 0;
                data[(j * width + i) * 3 + 2] = 255;
                break;
            }
            case 5:
            {
                data[(j * width + i) * 3 + 0] = 255;
                data[(j * width + i) * 3 + 1] = 0;
                data[(j * width + i) * 3 + 2] = 0;
                break;
            }
            case 6:
            {
                data[(j * width + i) * 3 + 0] = 0;
                data[(j * width + i) * 3 + 1] = 0;
                data[(j * width + i) * 3 + 2] = 255;
                break;
            }
            case 7:
            {
                data[(j * width + i) * 3 + 0] = 0;
                data[(j * width + i) * 3 + 1] = 0;
                data[(j * width + i) * 3 + 2] = 0;
                break;
            }
            default:
                break;
            }
        }
    }
    fwrite(data, width * heigth * 3, 1, fp);
    fclose(fp);
    free(data);
    return 0;
}

int main()
{
    //yuv420_split("lena_256x256_yuv420p.yuv", 256, 256, 1);
    //yuv444_split("lena_256x256_yuv444p.yuv", 256, 256, 1);
    //yuv420_gray("lena_256x256_yuv420p.yuv", 256, 256, 1);
    //yuv420p_halfy("lena_256x256_yuv420p.yuv", 256, 256, 1);
    //yuv420_border("lena_256x256_yuv420p.yuv", 256, 256, 20, 1);
    //yuv420_garybar(800, 600, 0, 255, 100, "graybar_800_600.yuv");
    //yuv420_psnr("lena_256x256_yuv420p.yuv", "lena_distort_256x256_yuv420p.yuv", 256, 256, 1);
    //rgb24_split("cie1931_500x500.rgb", 500, 500, 1);
    rgb24_to_bmp("lena_256x256_rgb24.rgb", 256, 256, "output_lena.bmp");
    //rgb24_to_yuv420("lena_256x256_rgb24.rgb", 256, 256, 1, "output_lena.yuv");
    //rgb24_colorbar(640, 360, W"colorbar_640x360.rgb");
    return 0;
}
