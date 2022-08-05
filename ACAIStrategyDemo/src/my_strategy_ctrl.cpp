#include "my_strategy.h"
#include "string.h"
#include <Python.h>
#include <iostream>
#include <string>
using namespace std;

void MyStrategy::onPKStart(const ACAI::PKConfig &pkConfig)
{
    memcpy(&mPKConfig, &pkConfig, sizeof(mPKConfig));
    // TODO : �˴�����û�������Ӧ������ʼ
}

void MyStrategy::onPKEnd()
{
    // TODO : �˴�����û�������Ӧ��������
	initData();
}

void MyStrategy::onPKOut(unsigned int flightID)
{
	// TODO : �˴�����û�������Ӧ�ѷ����ߵз��ɻ������л�����˳������¼�
}

void MyStrategy::backGround()
{
    // TODO : �˴�����û�����ִ�к�̨����
}

void MyStrategy::timeSlice20()
{
    // TODO : �˴�����û�����ִ��20ms��������
}

void MyStrategy::timeSlice40()
{
    // TODO : �˴�����û�����ִ��40ms��������
	ACAI::EventLog outputData;
	memset(&outputData, 0, sizeof(outputData));
	int a = ffunc(3,4);
	if(a == 7)
	{
		strcpy(outputData.EventDes, "Python link success!");
	}
	logEvent(outputData);
	/*if( mACMslWarning.mslCnt > 0 )
	{// ��β����
		DoTacHeadEvade();
		// ��־��ʾ
		strcpy(outputData.EventDes, "��β����");
		logEvent(outputData);
	}
	if(    0 == mACRdrTarget.tgtCnt 
		&& 0 == mACMslWarning.mslCnt 
		&& 0 == mACMSLInGuide.mslCnt 
		&& false == mACFCCStatus.envInfos[0].FPoleValid )
	{// ����ͻ��
		DoTacPointAtk();
		// ��־��ʾ
		strcpy(outputData.EventDes, "����ͻ��");
		logEvent(outputData);
	}
	if(    mACRdrTarget.tgtCnt > 0 
		&& 0 == mACMslWarning.mslCnt 
		&& mACMSLInGuide.mslCnt > 0  )
	{// ƫ���Ƶ�
		// ��־��ʾ
		strcpy(outputData.EventDes, "ƫ���Ƶ�");
		DoTacHeadGuide();
		logEvent(outputData);
	}
	if(    0 < mACRdrTarget.tgtCnt 
		&& 0 == mACMslWarning.mslCnt 
		&& 0 == mACMSLInGuide.mslCnt 
		&& 0 == mTeamMSLLaunched.mslCnt )
	{// ��������
		if( mACFCCStatus.envInfos[0].FPoleValid && ((int)mACFlightStatus.timeCounter) - (int)m_lastWpnShootTimeCounter > 10000 )
		{// ��������
			m_lastWpnShootTimeCounter = mACFlightStatus.timeCounter;
			DoTacWpnShoot();
			// ��־��ʾ
			strcpy(outputData.EventDes, "��������");
			logEvent(outputData);
		}
		else
		{
			// ��־��ʾ
			strcpy(outputData.EventDes, "��������");
			logEvent(outputData);
		}
		DoTacAltClimb();
	}*/
}

// ��������
void MyStrategy::DoTacWpnShoot()
{
	ACAI::WpnControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.launchPlaneID = mACFlightStatus.flightID; ///< ��������
    outputData.guidePlaneID	 = mACFlightStatus.flightID;  ///< �Ƶ������
    outputData.mslLockTgtID = mACRdrTarget.tgtInfos[0].tgtID;  ///< ����Ŀ����
    outputData._cmdCnt	= mACFlightStatus.timeCounter;       ///< ָ�����
	sendWpnControlCmd(outputData);
}

// ����ͻ��
void MyStrategy::DoTacPointAtk()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
    outputData.navCtrlCmd	= true;        ///< ��·����ָ��
    outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
	if( ACAI::V_FLIGHT_TEAM_RED == mACFlightStatus.flightTeam )
	{
		outputData.desireNavLon	= mPKConfig.RedMissionLon;    ///< ������·����(rad)
		outputData.desireNavLat	= mPKConfig.RedMissionLat;    ///< ������·γ��(rad)
	}
	else
	{
		outputData.desireNavLon	= mPKConfig.BlueMissionLon;    ///< ������·����(rad)
		outputData.desireNavLat	= mPKConfig.BlueMissionLat;    ///< ������·γ��(rad)
	}
    outputData.desireNavAlt = 2000;    ///< ������·�߶�(m)
    outputData.desireSpeed	= 200;     ///< ������·�ٶ�(m/s)
    outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
	sendFlyControlCmd(outputData);
}
// ��β����
void MyStrategy::DoTacHeadEvade()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
	outputData.altCtrlCmd = true;        ///< �߶ȱ���ָ��
    outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
    outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
    outputData.desireAlt = 2000;       ///< �����߶�(m)
    outputData.desireHead = mACMslWarning.threatInfos[0].azGeo + PI;///< ������·����(rad)
    outputData.desireSpeed = 200;///< ������·�ٶ�(m/s)
    outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
	sendFlyControlCmd(outputData);
}
// ƫ���Ƶ�
void MyStrategy::DoTacHeadGuide()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
	outputData.altCtrlCmd = true;        ///< �߶ȱ���ָ��
    outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
    outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
    outputData.desireAlt = 2000;       ///< �����߶�(m)
    outputData.desireHead = mACMSLInGuide.guideInfos[0].mslGuideAz+35*PI/180;///< ������·����(rad)
    outputData.desireSpeed = 200;///< ������·�ٶ�(m/s)
    outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
	sendFlyControlCmd(outputData);
}
// ��������
void MyStrategy::DoTacAltClimb()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
	outputData.altCtrlCmd = true;        ///< �߶ȱ���ָ��
    outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
    outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
    outputData.desireAlt = 4000;       ///< �����߶�(m)
    outputData.desireHead = mACRdrTarget.tgtInfos[0].azGeo;///< ������·����(rad)
    outputData.desireSpeed = 200;///< ������·�ٶ�(m/s)
    outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
	sendFlyControlCmd(outputData);
}

int ffunc(int a, int b)
{
	Py_Initialize();
	if(!Py_IsInitialized())
	{
		cout << "[Error] Init error" << endl;
		return -1;
	}

	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('./')");
	PyObject *pName, *pModule, *pFunc, *pArgs, *pRet;

	pName = PyUnicode_FromString("func");
	pModule = PyImport_Import(pName);

	if(!pModule)
	{
		cout << "[Error] Import module error" << endl;;
		return -1;
	}

	cout << "[INFO] Import module success" << endl;

	pFunc = PyObject_GetAttrString(pModule, "func");
	
	if(!pFunc)
	{
		cout << "[Error] Import function error" << endl;
		return -1;
	}

	cout << "[INFO] Get function success" << endl;

	pArgs = PyTuple_New(2);

	PyTuple_SetItem(pArgs, 0, Py_BuildValue("i", a));
	PyTuple_SetItem(pArgs, 1, Py_BuildValue("i", b));

	pRet = PyObject_CallObject(pFunc, pArgs);
	int res = 0;
	if(pRet)
	{
		PyArg_Parse(pRet, "i", &res);
	}

	Py_DecRef(pName);
	Py_DecRef(pModule);
	Py_DecRef(pFunc);
	Py_DecRef(pArgs);
	Py_DecRef(pRet);
	Py_Finalize();

	return res;
}