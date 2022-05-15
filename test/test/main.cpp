#include <windows.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <memory>
//#include <boost/progress.hpp>
#include <vector>
#include <string>
#include <stdlib.h>
#include<algorithm>
#include <Mmsystem.h>
#include "pugixml.hpp"
#include <opencv2/opencv.hpp>
#include <functional>

#pragma comment(lib, "Winmm.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define XRD_MAX_ROCK   (500)
using namespace std;

#pragma comment(lib, "ws2_32.lib")

WORD wVersinRequested;
WSADATA wsaData;
int err;
SOCKADDR_IN addrSrv;//�趨�������˵�IP�Ͷ˿�
SOCKET sockClient;
char recvBuf[100] = { 0 };
char sendBuf[100] = { 0 };

bool SocketInit()
{
	wVersinRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersinRequested, &wsaData);
	if (err != 0)
	{
		cout << "����ʹ��Winsock DLLʧ��" << endl;
		return false;
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		cout << "����ʹ��Winsock DLLʧ��" << endl;
		return false;
	}
	return true;
}

int SocketConnect(char* nServerIp, unsigned int nPort)
{
	int ret = 0;
	u_long iMode;
	addrSrv.sin_addr.S_un.S_addr = inet_addr(nServerIp);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(nPort);
	sockClient = socket(AF_INET, SOCK_STREAM, 0);//���������������Ϊ0�������Զ�ѡ��Э�顣
	bind(sockClient, (sockaddr*)&addrSrv, sizeof(sockaddr));
	ioctlsocket(sockClient, FIONBIO, &iMode);//��conne  recv ���óɷ�����
	ret = connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));//��������������ӡ�

	return ret;
}

bool SocketSend(char *Buff, unsigned int nSize)
{

	//for (int i = 0; i < 20; i++)//he 0915
	//{
	//	std::cout << " " << (int)Buff[i];
	//}
	//std::cout << " send---" << std::endl;

	memcpy(sendBuf, Buff, nSize);
	int ret = send(sockClient, sendBuf, nSize, 0);
	if (ret <= 0)
		return false;
	return true;
}

int DisConnectLink()
{
	int ret = closesocket(sockClient);
	WSACleanup();
	return ret;
}

void fun()
{
	int retval, optlen;
	cout << "hello.." << endl;
	SocketInit();
	SocketConnect("192.168.26.37", 60000);
	SocketSend("hello...", 50);
	optlen = sizeof(int);
	std::thread nThread([&]() {
		while (true)
		{
			//int ret = getsockopt(sockClient, SOL_SOCKET, SO_KEEPALIVE, (char*)&retval, &optlen);
			//printf("ret==%d___retval::%ld\n", ret, retval);
			//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			bool ret = SocketSend("hello...", 50);
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			if (!ret)
			{
				cout << "send  error!!" << endl;
			}
		}
	});
	nThread.detach();
	getchar();
	DisConnectLink();
	system("pause");
}

int  main00()
{
	cout << "hello.." << endl;
	auto str = make_shared<std::string>("dasdasd");
	cout << str->c_str() << endl;
	system("pause");
	return 0;
}


constexpr uint8_t ROCKMSGBOX = 10;//һ����ʮ������
constexpr uint8_t GRAYLEVEL = 50;
constexpr uint8_t GRAYLEVEL2 = 10;//ÿ�������µûҶȼ���

void WINAPI TimeCallbackFuction(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2)
{
	//��Ҫִ�еĲ���
	std::thread nThread([&] {
		cout << "start" << endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		cout << "end" << endl;

	});
	nThread.detach();
}

typedef enum {
	XRD_HORL_TL = 0,
	XRD_HORL_TH
}XRD_HORL_TYPE;


typedef enum {
	XRD_ROCK_BAD = 0,
	XRD_ROCK_GOOD
}XRD_ROCK_TYPE;

typedef enum {
	XRD_FAILED = -1,
	XRD_SUCCEED
}XRD_STATUS_TYPE;

typedef enum {
	XRAY_OK = 0, //������OK
	XRAY_FAULT_ONE, //һ���������л���
	XRAY_FAULT_TWO, //�����������л���
	XRAY_FAULT //���ģ�������
}XRAY_STATUS;

typedef enum {
	XRD_TYPE_16U1 = 0,
	XRD_TYPE_8U1,
	XRD_TYPE_8U3
}XRD_IMAGE_TYPE;

typedef enum {
	XRAY_DT = 0, //DT������
	XRAY_SENSTECH //SENSTECH������
}XRAY_Sensor;

typedef struct
{
	unsigned long time;     //!< Timestamp [ms]
	int width;
	int height;
	int widthStep;
	XRD_IMAGE_TYPE eType;   //!< type
	char *pData;            //!< imageData  void
} DETECTED_DATA;

typedef struct
{
	int x;
	int y;
}SRD_POINT;

typedef struct
{
	float x;
	float y;
}SRF_POINT;

typedef struct
{
	int top;
	int down;
	int left;
	int right;
}SRD_RECT;

struct SRD_Rock
{
	XRD_ROCK_TYPE eType;
	SRD_RECT rectOri;      //��ʯͼ������
	SRD_RECT rectCorrect;  //��ʯ������ʵ������
	SRD_POINT middle;      //��ʯͼ������
	SRD_POINT centroid;    //��ʯԲ��
	float dAveTH{ 0 };
	float dAveTL{ 0 };
	float fTHMin{ 0 };
	float fTLMin{ 0 };
	int flag;
	float fR_ave;
	float fSD;
	float fCVR;
	float fRadius;
	float fArea;
};

typedef struct
{
	unsigned long time;     //!< Timestamp [ms]
	int numRock;
	SRD_Rock stRock[XRD_MAX_ROCK];
} XRD_Result;

struct GrayRange
{
	uint16_t GrayUp{ 0 };//�Ҷ�����
	uint16_t GrayDown{ 0 };//�Ҷ�����
};

struct RockMsgBox
{
	//float fQualityYield{ 0 };//���� β������/������
	float fYield{ 0 };//���Ⱥи���
	//Gray ������
	uint16_t nUp{ 0 };//��¼���Ⱥ�������Ҷȵ�����
	uint16_t nDown{ 0 };//��¼���Ⱥ�������Ҷȵ�����
	uint8_t nMid{ 0 };//��¼���ӵı��
	vector<SRD_Rock> m_SRD_RockArr[GRAYLEVEL];//��ſ�ʯ�Ļ�����Ϣ
	vector<SRD_Rock> m_SRD_RockArr2[GRAYLEVEL2];
	float fProbability[GRAYLEVEL2]{ 0 };
	GrayRange m_GrayRange[GRAYLEVEL2];//����Ҷ�������(10��)
	uint16_t m_GrayBorder[GRAYLEVEL2]{ 0 };//ÿ�������µ÷ֱ�Ҷȼ��þ��߽߱�(TLMin)
public:
	uint64_t GetRockNum()//��ȡ��ʯ������
	{
		uint64_t nRocknum = 0;
		for (int i = 0; i < GRAYLEVEL; i++)
		{
			nRocknum += m_SRD_RockArr[i].size();
		}
		return nRocknum;
	}
	uint64_t GetRockNum2()//��ȡ��ʯ������
	{
		uint64_t nRocknum = 0;
		for (int i = 0; i < GRAYLEVEL2; i++)
		{
			nRocknum += m_SRD_RockArr2[i].size();
		}
		return nRocknum;
	}
};

class Ore
{
public:
	void GetRockMsg(string sFilename);
	void SetRockMsgBoxYield(float fSup, float fMid, float fInfimum, float fgain);//����ÿ����ʯ���ӵ��׷�
	void CheckRockMsgBoxBoundaries();
	void ResetRockMsgBoxData();//����Gray����ʯ��Ϣ���·ֳ�10��
	void CalculateGrayProbability();//����ÿ���Ҷȵĸ���
	void WriteXml(char *filename);//test
	void CalculateGrayBorder();
	void RestProbability(uint8_t nBoxNum, float fProbability);//�������ú����ڵĸ���
	float CheckRockBoxProbability(uint8_t nBoxNum);//�鿴ĳһ����������Ŀ�ʯ�׷�
private:
	void Sorting(vector<SRD_Rock> &nRock);
	void CalculateGrayUpAndDown();//���������ĻҶ�������
	uint8_t getRockMsgBoxMaxSize();
	int getThrd(int nR);
private:
	RockMsgBox m_RockMsgBox[ROCKMSGBOX];
	float m_fGain{ 0 };//Gray ��������
};



void Ore::GetRockMsg(string sFilename)
{
	cout << sFilename.c_str() << endl;
	FILE *fp;
	uint64_t num = 0;
	XRD_Result nResult;
	fp = fopen(sFilename.c_str(), "rb");
	if (!fp)
	{
		printf("error!\n");
		return;
	}
	while (!feof(fp))
	{
		fread(&nResult, sizeof(XRD_Result), 1, fp);
		for (int i = 0; i < nResult.numRock;i++)
		{
			int nl = getThrd((int)nResult.stRock[i].fRadius);
			for (int j = 0; j < GRAYLEVEL; j++)//���������ٰ��Ҷ����ֳ�GRAYLEVEL���ȼ�
			{
				float TLAveBuf = nResult.stRock[i].dAveTL;
				if (TLAveBuf < (j + 1) * 1000 && TLAveBuf > j * 1000)
					m_RockMsgBox[nl / 2].m_SRD_RockArr[j].push_back(nResult.stRock[i]);
			}
		}
		num += nResult.numRock;
	}
	cout << "num--" << num << endl;
	fclose(fp);
}

void Ore::SetRockMsgBoxYield(float fSup, float fMid, float fInfimum, float fgain)
{
	uint8_t nl = getRockMsgBoxMaxSize();
	m_RockMsgBox[nl].fYield = fMid;//��ʯ���ĺ���ʹ�ܳ��м���� 
	m_RockMsgBox[0].fYield = fSup;
	m_RockMsgBox[9].fYield = fInfimum;
	m_fGain = fgain;
	for (int i = nl; i>0; i--)
	{
		m_RockMsgBox[i].fYield = ((fSup - fMid) / nl) * (nl - i) + fMid;
	}
	for (int i = nl; i < 9; i++)
	{
		m_RockMsgBox[i].fYield = ((fMid - fInfimum) / (9-nl)) * (9 - i) + fInfimum;
	}
	//reset ÿ�����Ⱥ��ӵĸ��ʾ�����
	
	CalculateGrayUpAndDown();
}

void Ore::CheckRockMsgBoxBoundaries()
{
	//for (int i = 0; i < 10; i++)
	//{
	//	m_RockMsgBox[i].m_SRD_Rock.size();
	//}
}

void Ore::ResetRockMsgBoxData()
{
	vector<SRD_Rock> nSRD_Rockbuff;
	for (int i = 0; i < ROCKMSGBOX; i++)
	{
		for (int j = 0; j < GRAYLEVEL; j++)
		{
			nSRD_Rockbuff.insert(nSRD_Rockbuff.end(), m_RockMsgBox[i].m_SRD_RockArr[j].begin(),
				m_RockMsgBox[i].m_SRD_RockArr[j].end());
		}
		Sorting(nSRD_Rockbuff);
		uint32_t nLevel = (m_RockMsgBox[i].nUp - m_RockMsgBox[i].nDown) / 10;
		for (int j = 0; j < nSRD_Rockbuff.size(); j++)
		{
			uint16_t nTLAve = nSRD_Rockbuff.at(j).dAveTL;
			for (int k = 0; k < GRAYLEVEL2; k++)
			{
				uint16_t downtmp = m_RockMsgBox[i].nDown + nLevel * k;		//������
				uint16_t uptmp = m_RockMsgBox[i].nDown + nLevel * (k + 1);	//������
				m_RockMsgBox[i].m_GrayRange[k].GrayDown = downtmp;
				m_RockMsgBox[i].m_GrayRange[k].GrayUp = uptmp;
				if (nTLAve > downtmp && nTLAve < uptmp)
				{
					m_RockMsgBox[i].m_SRD_RockArr2[k].push_back(nSRD_Rockbuff.at(j));
				}
			}
		}
		nSRD_Rockbuff.swap(vector<SRD_Rock>());
		uint32_t maxGray= m_RockMsgBox[i].m_SRD_RockArr2[0].size();
		for (int j = 0; j < GRAYLEVEL2; j++)
		{
			uint32_t bufGray = m_RockMsgBox[i].m_SRD_RockArr2[j].size();
			if (maxGray < bufGray)
			{
				maxGray = bufGray;
				m_RockMsgBox[i].nMid = j;
			}
		}
	}
	cout << "over" << endl;
}

void Ore::CalculateGrayProbability()
{
	for (int i = 0; i < ROCKMSGBOX; i++)
	{
		int nIndex = m_RockMsgBox[i].nMid;
		m_RockMsgBox[i].fProbability[nIndex] = m_RockMsgBox[i].fYield;
		float Graylow = m_RockMsgBox[i].fYield + m_fGain;
		float GrayHight = m_RockMsgBox[i].fYield - m_fGain;
		if (Graylow > 1)
			Graylow = 1;
		if (GrayHight < 0)
			GrayHight = 0;
		float ProbabilityLevel = (Graylow - m_RockMsgBox[i].fYield) / nIndex;//
		for (int j = 0; j < nIndex; j++)
		{
			m_RockMsgBox[i].fProbability[j] = Graylow - ProbabilityLevel*j;
		}
		ProbabilityLevel = (m_RockMsgBox[i].fYield - GrayHight) / (GRAYLEVEL2 - nIndex - 1);
		for (int j = GRAYLEVEL2 - 1; j > nIndex; j--)
		{
			m_RockMsgBox[i].fProbability[j] = GrayHight + (ProbabilityLevel * (GRAYLEVEL2 - 1 - j));
		}
	}
	cout << "over" << endl;
}

void Ore::WriteXml(char *filename)
{
	printf("make xmlV4 successful!!");

	std::string str = "-";
	pugi::xml_document xmlDoc;
	pugi::xml_node nodeSensorData = xmlDoc.append_child("XRT_PARTICLELEVEL_XML");
	// ����
	pugi::xml_node pre = xmlDoc.prepend_child(pugi::node_declaration);
	pre.append_attribute("algorithm") = "ParticleLevel";
	pre.append_attribute("version") = "4.0";
	// ע�ͽڵ�
	pugi::xml_node nodeCommentStudents = nodeSensorData.append_child(pugi::node_comment);
	nodeCommentStudents.set_value("AlgorithmParameter");
	pugi::xml_node nodeSensorList = nodeSensorData.append_child("XRT_CONFIG_PARTICLELEVEL");
	for (int i = 0; i < 10; i++)
	{
		pugi::xml_node nodeSensor = nodeSensorList.append_child("Particle");
		//nodeSensor.append_attribute("Level").set_value(i);
		char levelname[50]{ 0 };
		sprintf(levelname, "%d~%d", i * 10, (i + 1) * 10);
		nodeSensor.append_attribute("Level").set_value(levelname);
		nodeSensor.append_attribute("GrayMax").set_value(m_RockMsgBox[i].nUp);
		nodeSensor.append_attribute("GrayMin").set_value(m_RockMsgBox[i].nDown);
		int num = 0;
		for (int j = 0; j < 10; j++)
		{
			pugi::xml_node graylevel = nodeSensor.append_child("gray");
			char levelname[50]{ 0 };

			sprintf(levelname, "Level%d", num++);
			pugi::xml_attribute nodeLevel = graylevel.append_attribute(levelname);
			pugi::xml_attribute nodeLevelValues = graylevel.append_attribute("value");
			//pugi::xml_node nodeSensorTag0 = nodeSensor.append_child((std::to_string(j) + str + std::to_string(j + j * 100)).c_str());
			nodeLevel.set_value((std::to_string(m_RockMsgBox[i].m_GrayRange[j].GrayDown) + str + std::to_string(m_RockMsgBox[i].m_GrayRange[j].GrayUp)).c_str());
			nodeLevelValues.set_value(m_RockMsgBox[i].m_GrayBorder[j]);
		}
	}
	xmlDoc.save_file(filename, "\t", 1U, pugi::encoding_utf8);
	//xmlDoc.save_file("D:/atest.xml", "\t", 1U, pugi::encoding_utf8);
}

void Ore::CalculateGrayBorder()
{
	for (int i = 0; i < ROCKMSGBOX; i++)
	{
		for (int j = 0; j < GRAYLEVEL2; j++)
		{
			uint64_t nRockNum = m_RockMsgBox[i].m_SRD_RockArr2[j].size();//��ǰ����ĵ�ǰGray�µĿ�ʯ����
			float fProbability = m_RockMsgBox[i].fProbability[j];
			uint64_t nNums = fProbability * nRockNum;//�׵��ڼ����ʯ(Index)
			m_RockMsgBox[i].m_GrayBorder[j] = m_RockMsgBox[i].m_SRD_RockArr2[j].at(nNums - 1).fTLMin;
		}
	}
}

void Ore::RestProbability(uint8_t nBoxNum, float fProbability)
{
	if (nBoxNum > ROCKMSGBOX - 1)
		return;
	m_RockMsgBox[nBoxNum].fYield = fProbability;
}

float Ore::CheckRockBoxProbability(uint8_t nBoxNum)
{
	if (nBoxNum > ROCKMSGBOX - 1)
		return 0.f;
	//m_RockMsgBox[nBoxNum].m_GrayBorder
	double dJK = 0;//JK����
	double dWK = 0;//WK����
	for (int i = 0; i < ROCKMSGBOX; i++)
	{
		uint16_t nGrayBorader = m_RockMsgBox[nBoxNum].m_GrayBorder[i];
		for (int j = 0; j < m_RockMsgBox[nBoxNum].m_SRD_RockArr2[i].size(); j++)
		{
			if (m_RockMsgBox[nBoxNum].m_SRD_RockArr2[i].at(j).fTLMin > nGrayBorader)//ʶ��
			{
				//β��
				dWK += m_RockMsgBox[nBoxNum].m_SRD_RockArr2[i].at(j).fArea;
			}
			else
			{
				//����
				dJK += m_RockMsgBox[nBoxNum].m_SRD_RockArr2[i].at(j).fArea;
			}
		}
	}
	return (dWK / (dJK + dWK));
}

void Ore::Sorting(vector<SRD_Rock> &nRock)
{
	std::sort(nRock.rbegin(), nRock.rend(), [](SRD_Rock a, SRD_Rock b) {
		if (a.fTLMin > b.fTLMin)
			return true;
		return false;
	});
}

void Ore::CalculateGrayUpAndDown()
{
	for (int i = 0; i < ROCKMSGBOX; i++)
	{
		for (int j = 0; j < 50; j++)
		{
			if (m_RockMsgBox[i].m_SRD_RockArr[j].size() > 20)
			{
				m_RockMsgBox[i].nDown = j * 1000;
				break;
			}
		}
		for (int k = 49; k >= 0; k--)
		{
			if (m_RockMsgBox[i].m_SRD_RockArr[k].size() > 20)
			{
				m_RockMsgBox[i].nUp = (k + 1) * 1000;
				break;
			}
		}
	}
	cout << "over!!" << endl;
}



uint8_t Ore::getRockMsgBoxMaxSize()
{
	//int nRockNum = m_RockMsgBox[0].m_SRD_RockArr;
	int nRockNum = m_RockMsgBox[0].GetRockNum();
	int nMinnl = 0;
	for (int i = 0; i < 10; i++)
	{
		int nRockNumbuf = m_RockMsgBox[i].GetRockNum();
		cout << "nRockNumbuf--" << nRockNumbuf << endl;
		if (nRockNumbuf > nRockNum)
		{
			nRockNum = nRockNumbuf;
			nMinnl = i;
		}
	}
	return nMinnl;
}

int Ore::getThrd(int nR)
{
	int n = 0;
	int nTmp = 5;

	for (int i = 0; i < 20 - 1; i++) {
		if (nR < nTmp) {
			break;
		}
		n++;
		nTmp += 5;
	}
	return n;
}



int main000()
{
	//timeSetEvent(200, 0, (LPTIMECALLBACK)&TimeCallbackFuction, 1, TIME_PERIODIC);
	
	Ore nOre;
	nOre.GetRockMsg("D:/st.dat");
	nOre.SetRockMsgBoxYield(0.1, 0.1, 0.1, 0);
	nOre.ResetRockMsgBoxData();//����װ��
	//�˴�����������ӵĸ���
	//nOre.RestProbability(3, 0.9);
	nOre.CalculateGrayProbability();//����Ҷȸ���
	nOre.CalculateGrayBorder();//��������߽߱�
	nOre.WriteXml("D:/atest0.xml");

	for (int i = 0; i < 10; i++)
	{
		double gl = nOre.CheckRockBoxProbability(i);
		cout << "gl--" << gl << endl;
	}
	cout << ".............." << endl;
	nOre.RestProbability(3, 0.9);
	nOre.CalculateGrayProbability();//����Ҷȸ���
	nOre.CalculateGrayBorder();//��������߽߱�
							   //nOre.WriteXml();
	nOre.WriteXml("D:/atest1.xml");
	for (int i = 0; i < 10; i++)
	{
		double gl = nOre.CheckRockBoxProbability(i);
		cout << "gl--" << gl << endl;
	}
	//std::vector<int> v1;
	//std::vector<int> v2;
	//for (int i = 0; i < 10; i++)
	//{
	//	v1.push_back(i);
	//}
	//for (int i = 0; i < 10; i++)
	//{
	//	v2.insert(v2.end(), v1.begin(), v1.end());
	//}
	//cout << "over" << endl;
	system("pause");
	return 0;
}


template <typename T>
void ClearVectorMemory(vector<T> &nSwap) { nSwap.swap(vector<T>()); }

void fun(uint32_t ntype)
{
	switch (ntype)
	{
	case 1:
		cout << "1" << endl;
	case 2:
		cout << "2" << endl;
	}
}

//int main()
//{
//	int ret;
//	int len;
//	SOCKADDR_IN m_addrSrv;//�趨�������˵�IP�Ͷ˿�
//	SOCKET m_SocketSever;
//	SocketInit();
//	m_SocketSever = socket(AF_INET, SOCK_STREAM, 0);
//	m_addrSrv.sin_family = AF_INET;
//	m_addrSrv.sin_port = htons(atoi("2000"));
//	m_addrSrv.sin_addr.S_un.S_addr = inet_addr("192.168.0.20");
//	//ret = bind(m_SocketSever, (LPSOCKADDR)&m_addrSrv, sizeof(SOCKADDR_IN));
//	bind(m_SocketSever, (sockaddr *)&m_addrSrv, sizeof(SOCKADDR_IN));
//	ret = listen(m_SocketSever, 10);
//	m_SocketSever = accept(m_SocketSever, (SOCKADDR *)&m_addrSrv, &len);
//
//
//	int iSend = send(m_SocketSever, "1234", 100, 0);
//	system("pause");
//	return 0;
//}


#include <iostream>
#include <string>
#include <winsock.h>
using namespace std;
#pragma comment(lib, "ws2_32.lib")

using ThreadFun = std::function<void*(void*)>;

int main()
{
	//WSADATA WSAdata;
	//if (!WSAStartup(MAKEWORD(2, 2), &WSAdata))
	//{
	//	cout << "��ʼ���ɹ�" << endl;
	//}
	//else
	//{
	//	cout << "��ʼ��ʧ��" << endl;
	//	exit(1);
	//}
	//SOCKET server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	//if (server_sock == INVALID_SOCKET)
	//{
	//	cout << "socket fail" << endl;
	//	exit(1);
	//}
	////��
	//SOCKADDR_IN server_addr;
	//server_addr.sin_family = AF_INET;
	//server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	//server_addr.sin_port = htons(8080);
	//if (SOCKET_ERROR == ::bind(server_sock, (sockaddr *)&server_addr, sizeof(server_addr)))
	//{
	//	cout << "bind error" << endl;
	//}
	//else
	//	cout << "bind ok" << endl;


	//if (listen(server_sock, 13) == SOCKET_ERROR)
	//{
	//	cout << "listen error" << endl;
	//}
	//else
	//{
	//	cout << "listen ok" << endl;
	//}

	//SOCKADDR_IN client_addr;
	//SOCKET client_sock;
	//int cAddrLen = sizeof client_addr;
	//client_sock = accept(server_sock, (sockaddr *)&client_addr, &cAddrLen);
	//if (client_sock == INVALID_SOCKET)
	//{
	//	cout << "accept error" << WSAGetLastError() << endl;
	//}
	//else
	//	cout << "accept ok" << endl;

	//while (1)
	//{
	//	char buf[1024] = "";
	//	int num = recv(client_sock, buf, sizeof buf - 1, 0);
	//	if (num <= 0)
	//		break;
	//	cout << buf << endl;
	//}


	//closesocket(server_sock);
	//WSACleanup();
	ThreadFun nThreadFun = [&](void* input)->void* {
		cout << *(int*)(input) << endl;
		return input;
	};

	int ar = 100;
	int *b = (int *)nThreadFun(&ar);
	cout << "*b--" << *b << endl;

	system("pause");
	return 0;
}
