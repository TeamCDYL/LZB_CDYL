#include "my_strategy.h"
#include "string.h"
#include <iostream>
using namespace std;

//--------------------------------------
/// \brief ����������
void MyStrategy::maneuver_i(int fin,int sin)//һ����������������
{
	if (fin == 0) {
		switch(sin) {
		case 0: DoTacWpnShoot();break;
		case 1: DoTacToTar();break;
		case 2: DoTacAlt(0.5);break;
		case 3: DoTacAlt(0.4);break;
		case 4:	DoTacAlt(0.3);break;
		case 5: DoTacAlt(0.2);break;
		case 6: DoTacStaHov();break;
		case 7: DoTurnLeft30();break;
		case 8: DoTurnLeft60();break;
		case 9: DoTurnRight30();break;
		default: break;
		}
	} else {
		switch(sin) {
		case 0: DoTurnRight60();break;
		case 1: DoTacHeadEvade();break;
		case 2: DoTacCir();break;
		case 3: DoTacPointAtk();break;
		case 4: DoTurnFor();break;
		default: break;
		}
	}
}

void MyStrategy::write_maneuver(int fin,int sin)//һ����������������
{
	mCmd.fin = fin;
	mCmd.sin = sin;
	char * logContent = "";
	if (fin == 0) {
		switch(sin) {
		case 0: logContent = "[RACE] ������������л�";break;
		case 1: logContent = "[RACE] ��һ�ܵл�����";break;
		case 2: logContent = "[RACE] �߶�0.5";break;
		case 3: logContent = "[RACE] �߶�0.4";break;
		case 4:	logContent = "[RACE] �߶�0.3";break;
		case 5: logContent = "[RACE] �߶�0.2";break;
		case 6: logContent = "[RACE] ���λ���";break;
		case 7: logContent = "[RACE] ��תƫ��30";break;
		case 8: logContent = "[RACE] ��תƫ��60";break;
		case 9: logContent = "[RACE] ��תƫ��30";break;
		default: break;
		}
	} else {
		switch(sin) {
		case 0: logContent = "[RACE] ��תƫ��60 ";break;
		case 1: logContent = "[RACE] ��ͷ";break;
		case 2: logContent = "[RACE] �ػ�";break;
		case 3: logContent = "[RACE] ��з����߷���";break;
		case 4: logContent = "[RACE] ��ǰ����";break;
		default: break;
		}
	}
	strcpy(mLog.EventDes, logContent);
	logEvent(mLog);
}

//--------------------------------------
/// ����������

/// \brief ��з����߷���
void MyStrategy::DoTacPointAtk()
{
	if (action_finished) {
		startCnt = (int)mACFlightStatus.timeCounter;
		action_finished = false;
	}
	if ((int)mACFlightStatus.timeCounter - startCnt < 2000) {
		ACAI::FlyControlCmd outputData;
		memset(&outputData, 0, sizeof(outputData));
		outputData.executePlaneID = mACFlightStatus.flightID;		 ///< Ŀ�ķɻ����
		outputData.navCtrlCmd	= true;			///< ��·����ָ��
		outputData.speedCtrlCmd = true;			///< �ٶȱ���ָ��
		outputData.altCtrlCmd = true;
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
		outputData.desireNavAlt = 6000;    ///< ������·�߶�(m)
		outputData.desireSpeed	= 1000;     ///< ������·�ٶ�(m/s)
		outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
		sendFlyControlCmd(outputData);
	}
	else {
		startCnt = (int)mACFlightStatus.timeCounter;
		action_finished = true;
	}
}

/// \brief ��һ�ܵл�����
void MyStrategy::DoTacToTar()
{
	if (action_finished) {
		startCnt = (int)mACFlightStatus.timeCounter;
		action_finished = false;
	}
	if ((int)mACFlightStatus.timeCounter - startCnt < 2000) {
		ACAI::FlyControlCmd outputData;
		memset(&outputData, 0, sizeof(outputData));
		outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
		outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
		outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
		if (GetNearestTgt().tgtID != 0) {
			outputData.headCtrlCmd = true;
			outputData.desireHead = GetNearestTgt().azGeo;
			outputData.altCtrlCmd = true;
			outputData.desireAlt = GetNearestTgt().alt;
		}
		sendFlyControlCmd(outputData);
	}
	else {
		startCnt = (int)mACFlightStatus.timeCounter;
		action_finished = true;
	}
}

/// \brief ����ָ���߶�
void MyStrategy::DoTacAlt(double rate)
{
	if (action_finished) {
		startCnt = (int)mACFlightStatus.timeCounter;
		action_finished = false;
	}
	if ((int)mACFlightStatus.timeCounter - startCnt < (int)fabs(mPKConfig.MaxFlyHeight * rate - mACFlightStatus.alt) * 1500) {
		ACAI::FlyControlCmd outputData;
		memset(&outputData, 0, sizeof(outputData));
		outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
		outputData.altCtrlCmd = true;        ///< �߶ȱ���ָ��
		outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
		outputData.desireAlt = mPKConfig.MaxFlyHeight * rate;       ///< �����߶�(m)
		outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
		outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
		sendFlyControlCmd(outputData);
	}
	else {
		startCnt = (int)mACFlightStatus.timeCounter;
		action_finished = true;
	}
}

/// \brief ���λ���
void MyStrategy::DoTacStaHov()
{
	if (action_finished) {
		startCnt = (int)mACFlightStatus.timeCounter;
		action_finished = false;
	}
	if ((int)mACFlightStatus.timeCounter - startCnt < 5000) {
		ACAI::FlyControlCmd outputData;
		memset(&outputData, 0, sizeof(outputData));
		outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
		outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
		outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
		if (mACFlightStatus.heading > 0)
			outputData.desireHead = mACFlightStatus.heading - 1.05;	///< ������·����(rad)
		else
			outputData.desireHead = mACFlightStatus.heading + 1.05;
		outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
		outputData.altCtrlCmd = true;
		outputData.desireAlt = mACFlightStatus.alt;
		outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
		sendFlyControlCmd(outputData);
	}
	else if ((int)mACFlightStatus.timeCounter - startCnt < 12000 && (int)mACFlightStatus.timeCounter - startCnt >= 5000) {
		ACAI::FlyControlCmd outputData;
		memset(&outputData, 0, sizeof(outputData));
		outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
		outputData.headCtrlCmd = true;
		outputData.desireHead = mACFlightStatus.heading;		///< ������·����(rad)
		outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
		outputData.altCtrlCmd = true;
		outputData.desireAlt = mACFlightStatus.alt;
		outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
		sendFlyControlCmd(outputData);
	}
	else if ((int)mACFlightStatus.timeCounter - startCnt < 14000 && (int)mACFlightStatus.timeCounter - startCnt >= 12000) {
		ACAI::FlyControlCmd outputData;
		memset(&outputData, 0, sizeof(outputData));
		outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
		outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
		outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
		if (mACFlightStatus.heading > 0)
			outputData.desireHead = mACFlightStatus.heading - 0.52;	///< ������·����(rad)
		else
			outputData.desireHead = mACFlightStatus.heading + 0.52;
		outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
		outputData.altCtrlCmd = true;
		outputData.desireAlt = mACFlightStatus.alt;
		outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
		sendFlyControlCmd(outputData);
	}
	else if ((int)mACFlightStatus.timeCounter - startCnt < 18000 && (int)mACFlightStatus.timeCounter - startCnt >= 14000) {
		ACAI::FlyControlCmd outputData;
		memset(&outputData, 0, sizeof(outputData));
		outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
		outputData.headCtrlCmd = true;
		outputData.desireHead = mACFlightStatus.heading;		///< ������·����(rad)
		outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
		outputData.altCtrlCmd = true;
		outputData.desireAlt = mACFlightStatus.alt;
		outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
		sendFlyControlCmd(outputData);
	}
	else if ((int)mACFlightStatus.timeCounter - startCnt < 23000 && (int)mACFlightStatus.timeCounter - startCnt >= 18000) {
		ACAI::FlyControlCmd outputData;
		memset(&outputData, 0, sizeof(outputData));
		outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
		outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
		outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
		if (mACFlightStatus.heading > 0)
			outputData.desireHead = mACFlightStatus.heading + 1.04;	///< ������·����(rad)
		else
			outputData.desireHead = mACFlightStatus.heading - 1.04;
		outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
		outputData.altCtrlCmd = true;
		outputData.desireAlt = mACFlightStatus.alt;
		outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
		sendFlyControlCmd(outputData);
	}
	else if ((int)mACFlightStatus.timeCounter - startCnt < 25000 && (int)mACFlightStatus.timeCounter - startCnt >= 23000) {
		ACAI::FlyControlCmd outputData;
		memset(&outputData, 0, sizeof(outputData));
		outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
		outputData.headCtrlCmd = true;
		outputData.desireHead = mACFlightStatus.heading;		///< ������·����(rad)
		outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
		outputData.altCtrlCmd = true;
		outputData.desireAlt = mACFlightStatus.alt;
		outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
		sendFlyControlCmd(outputData);
	}
	else if ((int)mACFlightStatus.timeCounter - startCnt < 29000 && (int)mACFlightStatus.timeCounter - startCnt >= 25000) {
		ACAI::FlyControlCmd outputData;
		memset(&outputData, 0, sizeof(outputData));
		outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
		outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
		outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
		if (mACFlightStatus.heading > 0)
			outputData.desireHead = mACFlightStatus.heading + 0.52;	///< ������·����(rad)
		else
			outputData.desireHead = mACFlightStatus.heading - 0.52;
		outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
		outputData.altCtrlCmd = true;
		outputData.desireAlt = mACFlightStatus.alt;
		outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
		sendFlyControlCmd(outputData);
	}
	else { 
		startCnt = (int)mACFlightStatus.timeCounter;
		action_finished = true;
	}
}

/// \brief ��תƫ��30
void MyStrategy::DoTurnLeft30()
{
	if (action_finished) {
		startCnt = (int)mACFlightStatus.timeCounter;
		action_finished = false;
	}
	if ((int)mACFlightStatus.timeCounter - startCnt < 2000) {
		ACAI::FlyControlCmd outputData;
		memset(&outputData, 0, sizeof(outputData));
		outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
		outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
		outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
		outputData.altCtrlCmd = true;
		outputData.desireAlt = mACFlightStatus.alt;
		if (mACFlightStatus.heading > 0)
			outputData.desireHead = mACFlightStatus.heading - 0.52;	///< ������·����(rad)
		else
			outputData.desireHead = mACFlightStatus.heading + 0.52;
		outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
		outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
		sendFlyControlCmd(outputData);
	}
	else { 
		startCnt = (int)mACFlightStatus.timeCounter;
		action_finished = true;
	}
}

/// \brief ��תƫ��60
void MyStrategy::DoTurnLeft60()
{
	if (action_finished) {
		startCnt = (int)mACFlightStatus.timeCounter;
		action_finished = false;
	}
	if ((int)mACFlightStatus.timeCounter - startCnt < 2000) {
		ACAI::FlyControlCmd outputData;
		memset(&outputData, 0, sizeof(outputData));
		outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
		outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
		outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
		outputData.altCtrlCmd = true;
		outputData.desireAlt = mACFlightStatus.alt;
		if (mACFlightStatus.heading > 0)
			outputData.desireHead = mACFlightStatus.heading - 1.05;	///< ������·����(rad)
		else
			outputData.desireHead = mACFlightStatus.heading + 1.05;
		outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
		outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
		sendFlyControlCmd(outputData);
	}
	else { 
		startCnt = (int)mACFlightStatus.timeCounter;
		action_finished = true;
	}
}

/// \brief ��תƫ��30
void MyStrategy::DoTurnRight30()
{
	if (action_finished) {
		startCnt = (int)mACFlightStatus.timeCounter;
		action_finished = false;
	}
	if ((int)mACFlightStatus.timeCounter - startCnt < 2000) {
		ACAI::FlyControlCmd outputData;
		memset(&outputData, 0, sizeof(outputData));
		outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
		outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
		outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
		outputData.altCtrlCmd = true;
		outputData.desireAlt = mACFlightStatus.alt;
		if (mACFlightStatus.heading > 0)
			outputData.desireHead = mACFlightStatus.heading + 0.52;	///< ������·����(rad)
		else
			outputData.desireHead = mACFlightStatus.heading - 0.52;
		outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
		outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
		sendFlyControlCmd(outputData);
	}
	else { 
		startCnt = (int)mACFlightStatus.timeCounter;
		action_finished = true;
	}
}

/// \brief ��תƫ��60
void MyStrategy::DoTurnRight60()
{
	if (action_finished) {
		startCnt = (int)mACFlightStatus.timeCounter;
		action_finished = false;
	}
	if ((int)mACFlightStatus.timeCounter - startCnt < 2000) {
		ACAI::FlyControlCmd outputData;
		memset(&outputData, 0, sizeof(outputData));
		outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
		outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
		outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
		outputData.altCtrlCmd = true;
		outputData.desireAlt = mACFlightStatus.alt;
		if (mACFlightStatus.heading > 0)	
			outputData.desireHead = mACFlightStatus.heading + 1.05;	///< ������·����(rad)
		else
			outputData.desireHead = mACFlightStatus.heading - 1.05;
		outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
		outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
		sendFlyControlCmd(outputData);
	}
	else { 
		startCnt = (int)mACFlightStatus.timeCounter;
		action_finished = true;
	}
}

/// \brief ��ͷ
void MyStrategy::DoTacHeadEvade()
{
	if (action_finished) {
		startCnt = (int)mACFlightStatus.timeCounter;
		action_finished = false;
	}
	if ((int)mACFlightStatus.timeCounter - startCnt < 4000) {
		ACAI::FlyControlCmd outputData;
		memset(&outputData, 0, sizeof(outputData));
		outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
		outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
		outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
		outputData.desireHead = mACFlightStatus.heading + PI;///< ������·����(rad)
		outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
		outputData.altCtrlCmd = true;
		outputData.desireAlt = mACFlightStatus.alt;
		outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
		sendFlyControlCmd(outputData);
	}
	else { 
		startCnt = (int)mACFlightStatus.timeCounter;
		action_finished = true;
	}
}

/// \brief ��ǰ����
void MyStrategy::DoTurnFor()
{
	if (action_finished) {
		startCnt = (int)mACFlightStatus.timeCounter;
		action_finished = false;
	}
	if ((int)mACFlightStatus.timeCounter - startCnt < 1000) {
		ACAI::FlyControlCmd outputData;
		memset(&outputData, 0, sizeof(outputData));
		outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
		outputData.headCtrlCmd = true;
		outputData.desireHead = mACFlightStatus.heading;		///< ������·����(rad)
		outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
		outputData.altCtrlCmd = true;
		outputData.desireAlt = mACFlightStatus.alt;
		outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
		sendFlyControlCmd(outputData);
	}
	else { 
		startCnt = (int)mACFlightStatus.timeCounter;
		action_finished = true;
	}
}

/// \brief �ػ�
void MyStrategy::DoTacCir() {
	if (action_finished) {
		startCnt = (int)mACFlightStatus.timeCounter;
		action_finished = false;
	}
	if ((int)mACFlightStatus.timeCounter - startCnt < 7000) {
		ACAI::FlyControlCmd outputData;
		memset(&outputData, 0, sizeof(outputData));
		outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
		outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
		outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
		outputData.desireHead = mACFlightStatus.heading + PI;///< ������·����(rad)
		outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
		outputData.altCtrlCmd = true;
		outputData.desireAlt = mACFlightStatus.alt;
		outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
		sendFlyControlCmd(outputData);
	}
	else { 
		startCnt = (int)mACFlightStatus.timeCounter;
		action_finished = true;
	}
}

//--------------------------------------
// ����������
///  \brief ��������
void MyStrategy::DoTacWpnShoot()
{
	if (action_finished) {
		startCnt = (int)mACFlightStatus.timeCounter;
		action_finished = false;
	}
	if ((int)mACFlightStatus.timeCounter - startCnt < 1000) {
		ACAI::WpnControlCmd outputData;
		memset(&outputData, 0, sizeof(outputData));
		outputData.launchPlaneID = mACFlightStatus.flightID;	///< ��������
		outputData.guidePlaneID	 = mACFlightStatus.flightID;	///< �Ƶ������

		outputData.mslLockTgtID = GetNearestTgt().tgtID;		///< ����Ŀ����
	
		outputData._cmdCnt	= mACFlightStatus.timeCounter;		///< ָ�����
		sendWpnControlCmd(outputData);
	}
	else { 
		startCnt = (int)mACFlightStatus.timeCounter;
		action_finished = true;
	}
}
