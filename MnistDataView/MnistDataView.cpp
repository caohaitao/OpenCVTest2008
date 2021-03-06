// MnistDataView.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "opencv2/opencv.hpp"

struct MnistViewData
{
	MnistViewData()
		:buffer(NULL)
        {

	}
	~MnistViewData()
	{	
                if (buffer){
                        free(buffer);
                        buffer = NULL;
                }
	}
	int width;
	int height;
	char * buffer;
	int buffer_size;
};

bool ReadMnistViewData(MnistViewData & d,char * file_path)
{
	FILE * f = fopen(file_path,"rb");
	if (!f){
		return false;
	}
	fread(&d.width,4,1,f);
	fread(&d.height,4,1,f);
        d.buffer_size = d.width*d.height;
        if (!d.buffer){
             d.buffer = (char*)malloc(d.buffer_size);   
        }
        fread(d.buffer,d.buffer_size,1,f);
	fclose(f);
        return true;
}
static void FillImage(IplImage * img,MnistViewData & d)
{
        for (int i=0;i<d.buffer_size;i++){
                unsigned char b = d.buffer[i];
                img->imageData[3*i] = d.buffer[i];
                img->imageData[3*i+1] = d.buffer[i];
                img->imageData[3*i+2] = d.buffer[i];
        }
}

int main(int argc, char* argv[])
{
        char * pp = argv[1];
        MnistViewData mvd;
        if (!ReadMnistViewData(mvd,pp)){
                return -1;
        }
        cvNamedWindow(pp, CV_WINDOW_AUTOSIZE);
        IplImage * pImage = NULL;
        CvSize cs;
        cs.width = mvd.width;
        cs.height = mvd.height;
        pImage = cvCreateImage(cs,8,3);
        FillImage(pImage,mvd);

        cvShowImage(pp,pImage);

        //如果不是0，则是等待多长时间，相当于sleep
        cvWaitKey();

        //释放图像内存
        cvReleaseImage(&pImage);
        //cvReleaseImage(&green_image);
        //销毁窗口
        cvDestroyAllWindows();
	return 0;
}

