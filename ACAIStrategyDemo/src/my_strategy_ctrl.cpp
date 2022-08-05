#include "my_strategy.h"
#include "string.h"
#include <Python.h>
#include <iostream>
#include <string>
using namespace std;

void MyStrategy::onPKStart(const ACAI::PKConfig &pkConfig)
{
    memcpy(&mPKConfig, &pkConfig, sizeof(mPKConfig));
    // TODO : 此处添加用户代码响应比赛开始
}

void MyStrategy::onPKEnd()
{
    // TODO : 此处添加用户代码响应比赛结束
	initData();
}

void MyStrategy::onPKOut(unsigned int flightID)
{
	// TODO : 此处添加用户代码响应友方或者敌方飞机被命中或出界退出房间事件
}

void MyStrategy::backGround()
{
    // TODO : 此处添加用户代码执行后台任务
}

void MyStrategy::timeSlice20()
{
    // TODO : 此处添加用户代码执行20ms周期任务
}

void MyStrategy::timeSlice40()
{
    // TODO : 此处添加用户代码执行40ms周期任务
	ACAI::EventLog outputData;
	memset(&outputData, 0, sizeof(outputData));
	int a = ffunc(3,4);
	if(a == 7)
	{
		strcpy(outputData.EventDes, "Python link success!");
	}
	logEvent(outputData);
	/*if( mACMslWarning.mslCnt > 0 )
	{// 置尾逃逸
		DoTacHeadEvade();
		// 日志显示
		strcpy(outputData.EventDes, "置尾逃逸");
		logEvent(outputData);
	}
	if(    0 == mACRdrTarget.tgtCnt 
		&& 0 == mACMslWarning.mslCnt 
		&& 0 == mACMSLInGuide.mslCnt 
		&& false == mACFCCStatus.envInfos[0].FPoleValid )
	{// 定向突防
		DoTacPointAtk();
		// 日志显示
		strcpy(outputData.EventDes, "定向突防");
		logEvent(outputData);
	}
	if(    mACRdrTarget.tgtCnt > 0 
		&& 0 == mACMslWarning.mslCnt 
		&& mACMSLInGuide.mslCnt > 0  )
	{// 偏置制导
		// 日志显示
		strcpy(outputData.EventDes, "偏置制导");
		DoTacHeadGuide();
		logEvent(outputData);
	}
	if(    0 < mACRdrTarget.tgtCnt 
		&& 0 == mACMslWarning.mslCnt 
		&& 0 == mACMSLInGuide.mslCnt 
		&& 0 == mTeamMSLLaunched.mslCnt )
	{// 爬升扩包
		if( mACFCCStatus.envInfos[0].FPoleValid && ((int)mACFlightStatus.timeCounter) - (int)m_lastWpnShootTimeCounter > 10000 )
		{// 武器发射
			m_lastWpnShootTimeCounter = mACFlightStatus.timeCounter;
			DoTacWpnShoot();
			// 日志显示
			strcpy(outputData.EventDes, "武器发射");
			logEvent(outputData);
		}
		else
		{
			// 日志显示
			strcpy(outputData.EventDes, "爬升扩包");
			logEvent(outputData);
		}
		DoTacAltClimb();
	}*/
}

// 武器发射
void MyStrategy::DoTacWpnShoot()
{
	ACAI::WpnControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.launchPlaneID = mACFlightStatus.flightID; ///< 发射机编号
    outputData.guidePlaneID	 = mACFlightStatus.flightID;  ///< 制导机编号
    outputData.mslLockTgtID = mACRdrTarget.tgtInfos[0].tgtID;  ///< 导弹目标编号
    outputData._cmdCnt	= mACFlightStatus.timeCounter;       ///< 指令计数
	sendWpnControlCmd(outputData);
}

// 定向突防
void MyStrategy::DoTacPointAtk()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
    outputData.navCtrlCmd	= true;        ///< 航路飞行指令
    outputData.speedCtrlCmd = true;      ///< 速度保持指令
	if( ACAI::V_FLIGHT_TEAM_RED == mACFlightStatus.flightTeam )
	{
		outputData.desireNavLon	= mPKConfig.RedMissionLon;    ///< 期望航路经度(rad)
		outputData.desireNavLat	= mPKConfig.RedMissionLat;    ///< 期望航路纬度(rad)
	}
	else
	{
		outputData.desireNavLon	= mPKConfig.BlueMissionLon;    ///< 期望航路经度(rad)
		outputData.desireNavLat	= mPKConfig.BlueMissionLat;    ///< 期望航路纬度(rad)
	}
    outputData.desireNavAlt = 2000;    ///< 期望航路高度(m)
    outputData.desireSpeed	= 200;     ///< 期望航路速度(m/s)
    outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}
// 置尾逃逸
void MyStrategy::DoTacHeadEvade()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.altCtrlCmd = true;        ///< 高度保持指令
    outputData.headCtrlCmd = true;       ///< 航向保持指令
    outputData.speedCtrlCmd = true;      ///< 速度保持指令
    outputData.desireAlt = 2000;       ///< 期望高度(m)
    outputData.desireHead = mACMslWarning.threatInfos[0].azGeo + PI;///< 期望航路航向(rad)
    outputData.desireSpeed = 200;///< 期望航路速度(m/s)
    outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}
// 偏置制导
void MyStrategy::DoTacHeadGuide()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.altCtrlCmd = true;        ///< 高度保持指令
    outputData.headCtrlCmd = true;       ///< 航向保持指令
    outputData.speedCtrlCmd = true;      ///< 速度保持指令
    outputData.desireAlt = 2000;       ///< 期望高度(m)
    outputData.desireHead = mACMSLInGuide.guideInfos[0].mslGuideAz+35*PI/180;///< 期望航路航向(rad)
    outputData.desireSpeed = 200;///< 期望航路速度(m/s)
    outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}
// 爬升扩包
void MyStrategy::DoTacAltClimb()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.altCtrlCmd = true;        ///< 高度保持指令
    outputData.headCtrlCmd = true;       ///< 航向保持指令
    outputData.speedCtrlCmd = true;      ///< 速度保持指令
    outputData.desireAlt = 4000;       ///< 期望高度(m)
    outputData.desireHead = mACRdrTarget.tgtInfos[0].azGeo;///< 期望航路航向(rad)
    outputData.desireSpeed = 200;///< 期望航路速度(m/s)
    outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
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