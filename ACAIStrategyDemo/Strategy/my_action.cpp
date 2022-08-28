#include "my_strategy.h"
#include "string.h"
#include <iostream>
using namespace std;

//--------------------------------------
/// \brief 基本动作集
void MyStrategy::maneuver_i(int fin,int sin)//一级索引，二级索引
{
	if (fin == 0) {
		switch(sin) {
		case 0: DoTacPointAtk();break;
		case 1: DoTacToTar();break;
		case 2:	DoTacAlt(0.6);break;
		case 3: DoTacAlt(0.5);break;
		case 4: DoTacAlt(0.4);break;
		case 5:	DoTacAlt(0.3);break;
		case 6: DoTacAlt(0.2);break;
		case 7:	DoTacAlt(0.1);break;
		case 8: DoTacStaHov();break;
		case 9: DoTurnLeft30();break;
		case 10: DoTurnLeft60();break;
		case 11: DoTurnRight30();break;
		case 12: DoTurnRight60();break;
		case 13: DoTacHeadEvade();break;
		case 14: DoTacCir();break;
		case 15: DoTurnFor();break;
		default: cout<<"[WARNING] 不存在的机动动作"<<endl;break;
		}
	} else {
		switch(sin) {
		case 0:DoTacWpnShoot();break;
		default: cout<<"[WARNING] 不存在的攻击动作"<<endl;break;
		}
	}
}

void MyStrategy::write_maneuver(int fin,int sin)//一级索引，二级索引
{
	mCmd.fin = fin;
	mCmd.sin = sin;
	char * logContent = "";
	if (fin == 0) {
		switch(sin) {
		case 0: logContent = "[RACE] 向敌方防线飞行";g_time_piece_length = 50;break;
		case 1: logContent = "[RACE] 向一架敌机飞行";g_time_piece_length = 50;break;
		case 2:	logContent = "[RACE] 高度0.6";g_time_piece_length = 400;break;
		case 3: logContent = "[RACE] 高度0.5";g_time_piece_length = 300;break;
		case 4: logContent = "[RACE] 高度0.4";g_time_piece_length = 100;break;
		case 5:	logContent = "[RACE] 高度0.3";g_time_piece_length = 100;break;
		case 6: logContent = "[RACE] 高度0.2";g_time_piece_length = 200;break;
		case 7:	logContent = "[RACE] 高度0.1";g_time_piece_length = 200;break;
		case 8: logContent = "[RACE] 蛇形机动";g_time_piece_length = 800;break;
		case 9: logContent = "[RACE] 左转偏置30";g_time_piece_length = 30;break;
		case 10: logContent = "[RACE] 左转偏置60";g_time_piece_length = 60;break;
		case 11: logContent = "[RACE] 右转偏置30";g_time_piece_length = 30;break;
		case 12: logContent = "[RACE] 右转偏置60 ";g_time_piece_length = 60;break;
		case 13: logContent = "[RACE] 掉头";g_time_piece_length = 180;break;
		case 14: logContent = "[RACE] 回环";g_time_piece_length = 360;break;
		case 15: logContent = "[RACE] 向前飞行";g_time_piece_length = 10;break;
		default: cout<<"[WARNING] 不存在的机动动作"<<endl;break;
		}
	} else {
		switch(sin) {
		case 0:logContent = "[RACE] 攻击距离最近敌机";g_time_piece_length = 10;break;
		default: cout<<"[WARNING] 不存在的攻击动作"<<endl;break;
		}
	}
	strcpy(mLog.EventDes, logContent);
	logEvent(mLog);
}

//--------------------------------------
/// 机动动作库
/// \brief 向敌方防线飞行
void MyStrategy::DoTacPointAtk()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID;		 ///< 目的飞机编号
	outputData.navCtrlCmd	= true;			///< 航路飞行指令
	outputData.speedCtrlCmd = true;			///< 速度保持指令
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
	outputData.desireNavAlt = 3000;    ///< 期望航路高度(m)
	outputData.desireSpeed	= 1000;     ///< 期望航路速度(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}

/// \brief 向一架敌机飞行
void MyStrategy::DoTacToTar()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	if (GetNearestTgt().tgtID != 0) {
		outputData.headCtrlCmd = true;
		outputData.desireHead = GetNearestTgt().azGeo;
		outputData.altCtrlCmd = true;
		outputData.desireAlt = GetNearestTgt().alt;
	}
	sendFlyControlCmd(outputData);
}

/// \brief 到达指定高度
void MyStrategy::DoTacAlt(double rate)
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.altCtrlCmd = true;        ///< 高度保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireAlt = mPKConfig.MaxFlyHeight * rate;       ///< 期望高度(m)
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}

/// \brief 蛇形机动
void MyStrategy::DoTacStaHov()
{
	static int startCnt = mACFlightStatus.timeCounter;
	if (mACFlightStatus.timeCounter - startCnt < 5000) {
		DoTurnLeft60();
	}
	else if (mACFlightStatus.timeCounter - startCnt < 12000 && mACFlightStatus.timeCounter - startCnt >= 5000) {
		DoTurnFor();
	}
	else if (mACFlightStatus.timeCounter - startCnt < 14000 && mACFlightStatus.timeCounter - startCnt >= 12000) {
		DoTurnLeft30();
	}
	else if (mACFlightStatus.timeCounter - startCnt < 18000 && mACFlightStatus.timeCounter - startCnt >= 14000) {
		DoTurnFor();
	}
	else if (mACFlightStatus.timeCounter - startCnt < 23000 && mACFlightStatus.timeCounter - startCnt >= 18000) {
		DoTurnRight60();
	}
	else if (mACFlightStatus.timeCounter - startCnt < 25000 && mACFlightStatus.timeCounter - startCnt >= 23000) {
		DoTurnFor();
	}
	else if (mACFlightStatus.timeCounter - startCnt < 29000 && mACFlightStatus.timeCounter - startCnt >= 25000) {
		DoTurnRight30();
	}
	else { 
		DoTurnFor();
		startCnt = mACFlightStatus.timeCounter;
	}
}

/// \brief 左转偏置30
void MyStrategy::DoTurnLeft30()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	if (mACFlightStatus.heading > 0)
		outputData.desireHead = mACFlightStatus.heading - 0.52;	///< 期望航路航向(rad)
	else
		outputData.desireHead = mACFlightStatus.heading + 0.52;
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}

/// \brief 左转偏置60
void MyStrategy::DoTurnLeft60()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	if (mACFlightStatus.heading > 0)
		outputData.desireHead = mACFlightStatus.heading - 1.05;	///< 期望航路航向(rad)
	else
		outputData.desireHead = mACFlightStatus.heading + 1.05;
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}

/// \brief 右转偏置30
void MyStrategy::DoTurnRight30()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	if (mACFlightStatus.heading > 0)
		outputData.desireHead = mACFlightStatus.heading + 0.52;	///< 期望航路航向(rad)
	else
		outputData.desireHead = mACFlightStatus.heading - 0.52;
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}

/// \brief 右转偏置60
void MyStrategy::DoTurnRight60()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	if (mACFlightStatus.heading > 0)	
		outputData.desireHead = mACFlightStatus.heading + 1.05;	///< 期望航路航向(rad)
	else
		outputData.desireHead = mACFlightStatus.heading - 1.05;
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}

/// \brief 掉头
void MyStrategy::DoTacHeadEvade()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireHead = mACFlightStatus.heading + PI;///< 期望航路航向(rad)
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}

/// \brief 向前飞行
void MyStrategy::DoTurnFor()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.headCtrlCmd = true;
	outputData.desireHead = mACFlightStatus.heading;		///< 期望航路航向(rad)
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}

/// \brief 回环
void MyStrategy::DoTacCir() {
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireHead = mACFlightStatus.heading + 2 * PI;///< 期望航路航向(rad)
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}

//--------------------------------------
// 攻击动作库
///  \brief 武器发射
void MyStrategy::DoTacWpnShoot()
{
	ACAI::WpnControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.launchPlaneID = mACFlightStatus.flightID;	///< 发射机编号
	outputData.guidePlaneID	 = mACFlightStatus.flightID;	///< 制导机编号

	outputData.mslLockTgtID = GetNearestTgt().tgtID;		///< 导弹目标编号
	
	outputData._cmdCnt	= mACFlightStatus.timeCounter;		///< 指令计数
	sendWpnControlCmd(outputData);
}
