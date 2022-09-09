#include "my_strategy.h"
#include "string.h"
#include <iostream>
using namespace std;

//--------------------------------------
/// \brief ����������
void MyStrategy::maneuver_i(int act)//һ����������������
{
	switch(act) {
		case 0: DoTacCir();break;
		case 1: DoTacToTar();break;
		case 2: DoTacAlt(0.5);break;
		case 3: DoTacAlt(0.4);break;
		case 4:	DoTacAlt(0.3);break;
		case 5: DoTacAlt(0.2);break;
		case 6: DoTacStaHov();break;
		case 7: DoTurnLeft30();break;
		case 8: DoTurnLeft60();break;
		case 9: DoTurnRight30();break;
		case 10: DoTurnRight60();break;
		case 11: DoTacHeadEvade();break;
		case 12: DoTacAlt(0.6);break;
		case 13: DoTacPointAtk();break;
		case 14: DoTurnFor();break;
		default: break;
	}
}

void MyStrategy::write_maneuver(int act)//һ����������������
{
	mCmd = act;
	char * logContent = "";
	switch(act) {
		case 0: logContent = "[RACE] �ػ�";break;
		case 1: logContent = "[RACE] ��һ�ܵл�����";break;
		case 2: logContent = "[RACE] �߶�0.5";break;
		case 3: logContent = "[RACE] �߶�0.4";break;
		case 4:	logContent = "[RACE] �߶�0.3";break;
		case 5: logContent = "[RACE] �߶�0.2";break;
		case 6: logContent = "[RACE] ���λ���";break;
		case 7: logContent = "[RACE] ��תƫ��30";break;
		case 8: logContent = "[RACE] ��תƫ��60";break;
		case 9: logContent = "[RACE] ��תƫ��30";break;
		case 10: logContent = "[RACE] ��תƫ��60 ";break;
		case 11: logContent = "[RACE] ��ͷ";break;
		case 12: logContent = "[RACE] �߶�0.6";break;
		case 13: logContent = "[RACE] ��з����߷���";break;
		case 14: logContent = "[RACE] ��ǰ����";break;
		default: break;
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
	if ((int)mACFlightStatus.timeCounter - startCnt < 5000) {
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
		outputData.desireNavAlt = 8000;    ///< ������·�߶�(m)
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
	if ((int)mACFlightStatus.timeCounter - startCnt < 5000) {
		ACAI::FlyControlCmd outputData;
		memset(&outputData, 0, sizeof(outputData));
		outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
		outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
		outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
		outputData.altCtrlCmd = true;
		if (GetNearestTgt().tgtID != 0) {
			outputData.headCtrlCmd = true;
			outputData.desireHead = GetNearestTgt().azGeo;
			outputData.desireAlt = GetNearestTgt().alt - 1500;
		}
		else {
			outputData.navCtrlCmd	= true;			///< ��·����ָ��
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
			outputData.desireNavAlt = 8000;    ///< ������·�߶�(m)
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
	if ((int)mACFlightStatus.timeCounter - startCnt < (int)fabs(mPKConfig.MaxFlyHeight * rate - mACFlightStatus.alt) * 1000) {
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
	if ((int)mACFlightStatus.timeCounter - startCnt < 4000) {
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
	if ((int)mACFlightStatus.timeCounter - startCnt < 4000) {
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
	if ((int)mACFlightStatus.timeCounter - startCnt < 8000) {
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
	if ((int)mACFlightStatus.timeCounter - startCnt < 16000) {
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
	if (mACRdrTarget.tgtCnt != 0 && mACFCCStatus.envInfos[0].FPoleValid == true && (mACFlightStatus.timeCounter - m_last_shoot_time) > 20000) {
		ACAI::WpnControlCmd outputData;
		memset(&outputData, 0, sizeof(outputData));
		outputData.launchPlaneID = mACFlightStatus.flightID;	///< ��������
		outputData.guidePlaneID	 = mACFlightStatus.flightID;	///< �Ƶ������

		unsigned int id = 0;
		if (mACRdrTarget.tgtCnt == 2)
			(mACRdrTarget.tgtInfos[0].slantRange < mACRdrTarget.tgtInfos[1].slantRange) ? (id = mACRdrTarget.tgtInfos[0].tgtID) : (id = mACRdrTarget.tgtInfos[1].tgtID);
		else
			id = mACRdrTarget.tgtInfos[0].tgtID;
		outputData.mslLockTgtID = id;							///< ����Ŀ����
	
		outputData._cmdCnt	= mACFlightStatus.timeCounter;		///< ָ�����
		sendWpnControlCmd(outputData);
		m_last_shoot_time = mACFlightStatus.timeCounter;
	}
}
