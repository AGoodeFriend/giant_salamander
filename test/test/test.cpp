#include "comm/taskqueue.hpp"
#include <iostream>
#include <math.h>
#include <windows.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include <map>
#include <queue>
#include <list>

using namespace std;
using namespace cv;
using namespace LPPCDEVICE;

constexpr int MAXDFNUMBER = 25;
constexpr int MAXONDFPOINTCOUNT = 256;
constexpr int MAXONELINEWIDTH = MAXONDFPOINTCOUNT * MAXDFNUMBER;
constexpr int MAXSENSORLINECOUNT = 512;

int mainwater()
{
	Mat img, imgGray, imgMask;
	Mat maskWaterShed;  // watershed()�����Ĳ���
	img = imread("D:/im.jpg");  //ԭͼ��
	//img.convertTo(imgGray, CV_8UC1);
	if (img.empty())
	{
		cout << "��ȷ��ͼ���ļ������Ƿ���ȷ" << endl;
		return -1;
	}
	cvtColor(img, imgGray, COLOR_BGR2GRAY);
	//GaussianBlur(imgGray, imgGray, Size(5, 5), 10, 20);  //ģ�����ڼ��ٱ�Ե��Ŀ
	//imgGray = imread("D:/image.tif", IMREAD_ANYDEPTH);
	//��ȡ��Ե�����б�����
	Canny(imgGray, imgMask, 150, 300);
	//Mat k = getStructuringElement(0, Size(3, 3));
	//morphologyEx(imgMask, imgMask, MORPH_CLOSE, k);

	imshow("��Եͼ��", imgMask);
	//imshow("ԭͼ��", img);

	//������ͨ����Ŀ
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(imgMask, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);

	//��maskWaterShed�ϻ�������,���������ˮ���㷨
	maskWaterShed = Mat::zeros(imgMask.size(), CV_32S);
	for (int index = 0; index < contours.size(); index++)
	{
		drawContours(maskWaterShed, contours, index, Scalar::all(index + 1),
			-1, 8, hierarchy, INT_MAX);
	}
	//��ˮ���㷨   ��Ҫ��ԭͼ����д���
	watershed(img, maskWaterShed);

	vector<Vec3b> colors;  // ������ɼ�����ɫ
	for (int i = 0; i < contours.size(); i++)
	{
		int b = theRNG().uniform(0, 255);
		int g = theRNG().uniform(0, 255);
		int r = theRNG().uniform(0, 255);
		colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
	}

	Mat resultImg = Mat(img.size(), CV_8UC3);  //��ʾͼ��
	for (int i = 0; i < imgMask.rows; i++)
	{
		for (int j = 0; j < imgMask.cols; j++)
		{
			// ����ÿ���������ɫ
			int index = maskWaterShed.at<int>(i, j);
			if (index == -1)  // ������ֵ����Ϊ-1���߽磩
			{
				resultImg.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
			}
			else if (index <= 0 || index > contours.size())  // û�б�������������Ϊ0 
			{
				resultImg.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
			}
			else  // ����ÿ�������ֵ���ֲ��䣺1��2������contours.size()
			{
				resultImg.at<Vec3b>(i, j) = colors[index - 1];  // ��Щ������Ƴɲ�ͬ��ɫ
			}
		}
	}
	imwrite("D:/maskWaterShed.tif", maskWaterShed);
	resultImg = resultImg * 0.6 + img * 0.4;
	imshow("��ˮ����", resultImg);

	//����ÿ�������ͼ��
	for (int n = 1; n <= contours.size(); n++)
	{
		Mat resImage1 = Mat(img.size(), CV_8UC3);  // ����һ�����Ҫ��ʾ��ͼ��
		for (int i = 0; i < imgMask.rows; i++)
		{
			for (int j = 0; j < imgMask.cols; j++)
			{
				int index = maskWaterShed.at<int>(i, j);
				if (index == n)
					resImage1.at<Vec3b>(i, j) = img.at<Vec3b>(i, j);
				else
					resImage1.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
			}
		}
		//��ʾͼ��
		//imshow(to_string(n), resImage1);
	}
	waitKey(0);
	return 0;
}
#if 0
class RawPicinfo
{
public:
	RawPicinfo();
	//Person(const Person& p)
	RawPicinfo(const RawPicinfo& _RawPicinfoPtr);
	~RawPicinfo();

	void setImage(uchar* image, int nBufferLen);
	void getImage(uchar* image, int &nBufferLen);
private:
	ushort *m_PicFromData = nullptr;//����ͼ��Ļ���
	int m_BufferLen{ 0 };
};

class RawImageCache
{
public:
	void AddImage(uchar* image, int nBufferLen);//���ͼ��
	uint64_t CheckState();
	void PopImage(uchar *image);
private:
	void ImageLink();
	std::queue<RawPicinfo> m_RawPicinfo;
};

int main()
{
	RawImageCache nRawImageCache;
	ushort* im = new ushort[MAXONELINEWIDTH * MAXSENSORLINECOUNT];
	Mat src2 = cv::imread("D:/im2.tif", cv::IMREAD_ANYDEPTH);
	Mat src1 = cv::imread("D:/im1.tif", cv::IMREAD_ANYDEPTH);

	Mat dec = Mat::zeros(src1.rows * 2, src1.cols, cv::IMREAD_ANYDEPTH);
	nRawImageCache.AddImage(src2.data, src2.cols * src2.rows * 2);
	cout << nRawImageCache.CheckState() << endl;
	nRawImageCache.AddImage(src1.data, src1.cols * src1.rows * 2);
	cout << nRawImageCache.CheckState() << endl;
	nRawImageCache.PopImage(dec.data);
	cout << nRawImageCache.CheckState() << endl;
	imshow("TT", dec);
	cv::waitKey(0);
	system("pause");
	return 0;
}

void RawImageCache::AddImage(uchar * image, int nBufferLen)
{
	RawPicinfo nRawPicinfo;
	nRawPicinfo.setImage(image, nBufferLen);
	m_RawPicinfo.push(nRawPicinfo);
	//nRawPicinfo.getImage(nullptr, 10);
	//m_RawPicinfo.back().getImage(nullptr, 10);
}

uint64_t RawImageCache::CheckState()
{
	return m_RawPicinfo.size();
}

void RawImageCache::PopImage(uchar * image)
{
	int nBufferLen;
	RawPicinfo nRawPicinfo1 = m_RawPicinfo.front();
	nRawPicinfo1.getImage(image, nBufferLen);
	m_RawPicinfo.pop();
	RawPicinfo nRawPicinfo2 = m_RawPicinfo.front();
	nRawPicinfo2.getImage(image + nBufferLen, nBufferLen);
}

RawPicinfo::RawPicinfo()
{
	if (m_PicFromData == nullptr)
	{
		m_PicFromData = new ushort[MAXONELINEWIDTH * MAXSENSORLINECOUNT];
	}
}

RawPicinfo::RawPicinfo(const RawPicinfo &_RawPicinfoPtr)
{
	if (m_PicFromData == nullptr)
	{
		m_PicFromData = new ushort[MAXONELINEWIDTH * MAXSENSORLINECOUNT];
	}
	m_BufferLen = _RawPicinfoPtr.m_BufferLen;
	memcpy(m_PicFromData, _RawPicinfoPtr.m_PicFromData, m_BufferLen);
}

RawPicinfo::~RawPicinfo()
{
	if (m_PicFromData != nullptr)
	{
		delete[] m_PicFromData;
		m_PicFromData = nullptr;
	}
}

void RawPicinfo::setImage(uchar * image, int nBufferLen)
{
	if (nBufferLen > MAXONELINEWIDTH * MAXSENSORLINECOUNT)
	{
		return;
	}
	m_BufferLen = nBufferLen;
	memcpy(m_PicFromData, image, nBufferLen);
}

void RawPicinfo::getImage(uchar * image, int &nBufferLen)
{
	if (image != nullptr)
	{
		nBufferLen = m_BufferLen;
		memcpy(image, m_PicFromData, m_BufferLen);
	}

}


#else
struct TargetLine
{
	uint64_t nLinePart1;
	uint64_t nLinePart2;
	uint64_t nLinePart3;
	uint64_t nLinePart4;
	TargetLine(uint64_t nPart1, uint64_t nPart2, uint64_t nPart3, uint64_t nPart4)
		:nLinePart1(nPart1), nLinePart2(nPart2), nLinePart3(nPart3), nLinePart4(nPart4) {}
	TargetLine()
		:nLinePart1(0), nLinePart2(0), nLinePart3(0), nLinePart4(0) {}
};

template<typename T>
void fun(T t, int a)
{
	cout << a << endl;
	cout << t.nLinePart1 << endl;
}
class taskqueuebuf
{
//public:
//	taskqueuebuf();
//	~taskqueuebuf();
public:
	void addTaskbuf(TargetLine t);
	void operationTaskbuf();
	uint64_t GetTaskSize();
	void getResult();
private:
	vector<TargetLine> m_addTask;
	vector<TargetLine> m_result;
};

int main()
{
	//TaskQueue<int> nTaskQueue;
	//nTaskQueue.initTaskQueue();
	//nTaskQueue.addTask(10, 1000 * 1000, [&](int a) { cout << a << endl; });
	TargetLine nTargetLine;
	taskqueuebuf ntaskqueuebuf;
	for (int i = 0; i < 10; i++)
	{
		ntaskqueuebuf.addTaskbuf(TargetLine{ 100, 20, 30, 40 });
	}
	for (int i = 10; i < 20; i++)
	{
		ntaskqueuebuf.addTaskbuf(TargetLine{ 200, 20, 30, 40 });
	}
	cout << "GetTaskSize--" << ntaskqueuebuf.GetTaskSize() << endl;
	ntaskqueuebuf.operationTaskbuf();
	ntaskqueuebuf.getResult();
	//fun(TargetLine{100, 20, 30, 40}, 10);
	system("pause");
	return 0;
}


#endif

void taskqueuebuf::addTaskbuf(TargetLine t)
{
	m_addTask.push_back(t);
}

void taskqueuebuf::operationTaskbuf()
{
	for (int i = 0; i < m_addTask.size() / 2; i++)
	{
		TargetLine TargetLinebuf;
		TargetLinebuf.nLinePart1 = m_addTask.at(i).nLinePart1 | m_addTask.at(i + m_addTask.size() / 2 - 1).nLinePart1;
		TargetLinebuf.nLinePart2 = m_addTask.at(i).nLinePart2 | m_addTask.at(i + m_addTask.size() / 2 - 1).nLinePart2;
		TargetLinebuf.nLinePart3 = m_addTask.at(i).nLinePart3 | m_addTask.at(i + m_addTask.size() / 2 - 1).nLinePart3;
		TargetLinebuf.nLinePart4 = m_addTask.at(i).nLinePart4 | m_addTask.at(i + m_addTask.size() / 2 - 1).nLinePart4;
		m_result.push_back(TargetLinebuf);
	}
	//m_result
}

uint64_t taskqueuebuf::GetTaskSize()
{
	return m_addTask.size();
}

void taskqueuebuf::getResult()
{
	for (int i = 0; i < m_result.size(); i++)
	{
		cout << m_result.at(i).nLinePart1 << endl;
		//cout << m_result.at(i).nLinePart2 << endl;
		//cout << m_result.at(i).nLinePart3 << endl;
		//cout << m_result.at(i).nLinePart4 << endl;
	}
}
