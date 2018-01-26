#include "stdafx.h"
#include "opencv2/opencv.hpp"
using namespace std;

void FIndReadImage(IplImage * src, IplImage * dst)
{
	//row
	for (int i = 0; i < src->height; i++)
	{
		//col
		for (int j = 0; j < src->width; j++)
		{
			int index_b = i*src->widthStep + j * 3;
			int index_g = index_b + 1;
			int index_r = index_b + 2;
			unsigned char b = src->imageData[index_b];
			unsigned char g = src->imageData[index_g];
			unsigned char r = src->imageData[index_r];
			if (r > 200 && g<50 && b<50)
			{
				dst->imageData[index_b] = 0;
				dst->imageData[index_g] = 0;
				dst->imageData[index_r] = 255;
			}
			else
			{
				dst->imageData[index_b] = 255;
				dst->imageData[index_g] = 255;
				dst->imageData[index_r] = 0;
			}

			//dst->imageDataOrigin[index_b] = src->imageDataOrigin[index_b];
			//dst->imageDataOrigin[index_g] = src->imageDataOrigin[index_g];
			//dst->imageDataOrigin[index_r] = src->imageDataOrigin[index_r];
		}
	}
}
//int main()
//{
//	const char * pstrImageName = "D:\\code\\OpenCVTest\\strain_result.bmp"/*"e:\\17code\\game\\snk3.bmp"*/;
//	//将图像加载到内存
//	IplImage * pImage = cvLoadImage(pstrImageName, CV_LOAD_IMAGE_UNCHANGED);
//
//	IplImage * copy_image = cvCreateImage(cvGetSize(pImage), 8, 3);
//	FIndReadImage(pImage, copy_image);
//
//	IplImage * pContourImg = pContourImg = cvCreateImage(cvGetSize(pImage), IPL_DEPTH_8U, 3);
//
//	IplImage * green_image = cvCreateImage(cvGetSize(pImage), 8, 1);
//	cvSetImageCOI(copy_image, 3);
//	cvCopy(copy_image, green_image);
//
//
//
//	IplImage * value_image = cvCreateImage(cvGetSize(pImage), 8, 1);
//	cvThreshold(green_image, value_image, 230, 255, CV_THRESH_BINARY);
//
//	CvMemStorage * storage = cvCreateMemStorage();
//	CvSeq * first_contour = NULL;
//	int Nc = cvFindContours(value_image, storage, &first_contour, sizeof(CvContour), CV_RETR_LIST);
//	CvSeq * c = NULL;
//	int max_are = -10;
//	CvBox2D max_box2d;
//	CvSeq * max_c = NULL;
//	for (c = first_contour; c != NULL; c = c->h_next)
//	{
//		CvBox2D temp_rect = cvMinAreaRect2(c);
//		int ar = temp_rect.size.width*temp_rect.size.height;
//		if (ar > max_are)
//		{
//			max_are = ar;
//			max_box2d = temp_rect;
//			max_c = c;
//		}
//	}
//
//	if (max_c)
//	{
//		int onetourlength = max_c->total;
//		//CvPoint * points = (CvPoint *)malloc(sizeof(CvPoint)*onetourlength);
//		CvSeqReader reader;
//		CvPoint pt = cvPoint(0, 0);
//		cvStartReadSeq(max_c, &reader);
//		for (int i = 0; i < onetourlength; i++)
//		{
//			CV_READ_SEQ_ELEM(pt, reader);
//			//points[i] = pt;
//			//printf("x=%d,y=%d\n", pt.x, pt.y);
//		}
//		CvScalar dcolor = CV_RGB(255, 0, 0);
//		CvScalar hcolor = CV_RGB(0, 0, 255);
//		cvDrawContours(pContourImg, max_c, dcolor, hcolor, 1, 1, 8);
//	}
//
//	cvNamedWindow("red", CV_WINDOW_AUTOSIZE);
//
//	cvShowImage("red", pContourImg);
//
//	//如果不是0，则是等待多长时间，相当于sleep
//	cvWaitKey();
//
//	//释放图像内存
//	cvReleaseImage(&pImage);
//	//cvReleaseImage(&green_image);
//	//销毁窗口
//	cvDestroyAllWindows();
//	return 1;
//}