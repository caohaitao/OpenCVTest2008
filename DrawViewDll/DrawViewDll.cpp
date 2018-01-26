// DrawViewDll.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "DVInterface.h"
#include <Windows.h>
#include "opencv2/opencv.hpp"
#include <queue>
using namespace std;


struct ViewData
{
	ViewData() : m_img_width(0)
		, m_img_height(0)
		, m_buffer(NULL)
		, m_last_sn(-1)
	{

	}
	~ViewData()
	{
		if (m_buffer) {
			delete[] m_buffer;
		}
	}
	int m_img_width;
	int m_img_height;
	char * m_buffer;
	int m_last_sn;
	char m_door_state;
	float m_result1;
	float m_result2;
};

ViewData m_vd;
IplImage * pImage = NULL;
CvFont font;
bool IsStop = false;
int data_size;
queue<char*> datas;
HANDLE g_hMutex;//声明一个handle

static void BufRead(void * src,void * dst, int dst_size, int * read_index)
{
	memcpy(dst, src, dst_size);
	(*read_index) += dst_size;
}
static bool VdInitFromFIle()
{
	FILE * f = fopen("d:\\CapViewData.txt", "r");
	if (!f) {
		return false;
	}
	int temp_sn;
	fread(&temp_sn, sizeof(int), 1, f);
	if (temp_sn<-1) {
		fclose(f);
		return false;
	}
	if (temp_sn == m_vd.m_last_sn) {
		fclose(f);
		return false;
	}
	m_vd.m_last_sn = temp_sn;
	int temp_width, temp_height;
	fread(&temp_width, sizeof(int), 1, f);
	fread(&temp_height, sizeof(int), 1, f);
	fread(&m_vd.m_door_state, sizeof(char), 1, f);
	fread(&m_vd.m_result1, sizeof(float), 1, f);
	fread(&m_vd.m_result2, sizeof(float), 1, f);
	if (m_vd.m_img_width == 0) {
		m_vd.m_img_width = temp_width;
		m_vd.m_img_height = temp_height;
	}
	else
	{
		if (temp_width != m_vd.m_img_width || temp_height != m_vd.m_img_height) {
			fclose(f);
			return false;
		}
	}
	if (!m_vd.m_buffer) {
		m_vd.m_buffer = new char[m_vd.m_img_width*m_vd.m_img_height * 3];
	}

	fread(m_vd.m_buffer, m_vd.m_img_width*m_vd.m_img_height * 3, 1, f);
	fclose(f);
	return true;
}
static bool VdInitFromQueue()
{
	char * p = NULL;
	WaitForSingleObject(g_hMutex,INFINITE);//上锁
	if (datas.size()==0){
		ReleaseMutex(g_hMutex);//解锁
		goto __failed;
	}
	p = datas.front();
	datas.pop();
	ReleaseMutex(g_hMutex);//解锁
	unsigned __int16 width;
	unsigned __int16 height;
	unsigned __int32 sn;
	int state;
	memcpy(&width,p,2);
	memcpy(&height,p+2,2);
	memcpy(&sn,p+4,4);
	memcpy(&state,p+8,4);
	int temp_sn = (int)sn;
	if (temp_sn<-1) {
		goto __failed;
	}
	if (temp_sn == m_vd.m_last_sn) {
		goto __failed;
	}

	m_vd.m_last_sn = temp_sn;
	m_vd.m_door_state = state;
	if (m_vd.m_img_width == 0) {
		m_vd.m_img_width = width;
		m_vd.m_img_height = height;
	}
	else
	{
		if (width != m_vd.m_img_width || height != m_vd.m_img_height) {
			goto __failed;
		}
	}
	if (!m_vd.m_buffer) {
		m_vd.m_buffer = new char[m_vd.m_img_width*m_vd.m_img_height * 3];
	}

	memcpy(m_vd.m_buffer,p+32,data_size-32);
	return true;
__failed:
	if (p){
		free(p);
	}
	return false;
}

void FillImage(IplImage * src)
{
	//for (int i=0;i<m_vd.m_img_height;i++){
	//	for (int j=0;j<m_vd.m_img_width;j++){
	//		int vd_index = (i*m_vd.m_img_width+j)*3;
	//		unsigned char r = m_vd.m_buffer[vd_index];
	//		unsigned char g = m_vd.m_buffer[vd_index+1];
	//		unsigned char b = m_vd.m_buffer[vd_index+2];
	//		src->imageData[vd_index] = b;
	//		src->imageData[vd_index+1] = g;
	//		src->imageData[vd_index+2] = r;
	//	}
	//}

	int pixnel_count = m_vd.m_img_height*m_vd.m_img_width;
	memcpy(src->imageData,m_vd.m_buffer,pixnel_count*3);

	unsigned char temp_c;
	for (int i=0;i<pixnel_count;i++){
		temp_c = src->imageData[3*i];
		src->imageData[3*i] = src->imageData[3*i+2];
		src->imageData[3*i+2] = temp_c;
	}

	CvScalar color =  cvScalar(0,0,255,1);

	char text[64]={0};
	sprintf(text,"sn=%d",m_vd.m_last_sn);

	cvPutText(src,text,cvPoint(10,20),&font,color);
	if (m_vd.m_door_state == 1){
		sprintf(text,"state:open");
	}
	else
	{
		sprintf(text,"state:close");
	}
	cvPutText(src,text,cvPoint(100,20),&font,color);

}


DWORD WINAPI ThreadClientAcceptFunc(LPVOID lpParam)
{

	while (true){
		if (IsStop){
			break;
		}
		//if (!VdInitFromFIle()){
		//	Sleep(40);
		//	continue;
		//}
		if (!VdInitFromQueue()){
			Sleep(20);
			continue;
		}
		if (pImage == NULL){
			CvSize cs;
			cs.width = m_vd.m_img_width;
			cs.height = m_vd.m_img_height;
			pImage = cvCreateImage(cs,8,3);
		}

		FillImage(pImage);

		cvShowImage("red", pImage);

		Sleep(20);
	}
	return 1;
}
DWORD WINAPI LoopMain(LPVOID lpParam)
{
	cvNamedWindow("red", CV_WINDOW_AUTOSIZE);
	cvInitFont(&font,CV_FONT_HERSHEY_DUPLEX,0.5,0.5,0,1,8);
	DWORD lpThreadId;
	HANDLE thread_handel = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadClientAcceptFunc,NULL,0,&lpThreadId);

	//如果不是0，则是等待多长时间，相当于sleep
	cvWaitKey();

	//释放图像内存
	cvReleaseImage(&pImage);
	//cvReleaseImage(&green_image);
	//销毁窗口
	cvDestroyAllWindows();
	return 1;
}

void run()
{
	g_hMutex = CreateMutex(NULL,FALSE,NULL);//创建互斥量
	IsStop = false;

	DWORD lpThreadId;
	HANDLE thread_handel = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)LoopMain,NULL,0,&lpThreadId);
}

void stop()
{
	IsStop = true;
	Sleep(1000);
	if (pImage){
		cvReleaseImage(&pImage);
	}
	cvDestroyAllWindows();
}


void PushData(char * p)
{
	WaitForSingleObject(g_hMutex,INFINITE);//上锁
	char * np = (char*)malloc(data_size);
	memcpy(np,p,data_size);
	datas.push(np);
	ReleaseMutex(g_hMutex);//解锁
}

void set_data_size(int s)
{
	data_size = s;
}