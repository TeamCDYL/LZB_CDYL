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
		default: cout<<"[WARNING] �����ڵĻ�������"<<endl;break;
		}
	} else {
		switch(sin) {
		case 0:DoTacWpnShoot();break;
		default: cout<<"[WARNING] �����ڵĹ�������"<<endl;break;
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
		case 0: logContent = "[RACE] ��з����߷���";g_time_piece_length = 50;break;
		case 1: logContent = "[RACE] ��һ�ܵл�����";g_time_piece_length = 50;break;
		case 2:	logContent = "[RACE] �߶�0.6";g_time_piece_length = 400;break;
		case 3: logContent = "[RACE] �߶�0.5";g_time_piece_length = 300;break;
		case 4: logContent = "[RACE] �߶�0.4";g_time_piece_length = 100;break;
		case 5:	logContent = "[RACE] �߶�0.3";g_time_piece_length = 100;break;
		case 6: logContent = "[RACE] �߶�0.2";g_time_piece_length = 200;break;
		case 7:	logContent = "[RACE] �߶�0.1";g_time_piece_length = 200;break;
		case 8: logContent = "[RACE] ���λ���";g_time_piece_length = 800;break;
		case 9: logContent = "[RACE] ��תƫ��30";g_time_piece_length = 30;break;
		case 10: logContent = "[RACE] ��תƫ��60";g_time_piece_length = 60;break;
		case 11: logContent = "[RACE] ��תƫ��30";g_time_piece_length = 30;break;
		case 12: logContent = "[RACE] ��תƫ��60 ";g_time_piece_length = 60;break;
		case 13: logContent = "[RACE] ��ͷ";g_time_piece_length = 180;break;
		case 14: logContent = "[RACE] �ػ�";g_time_piece_length = 360;break;
		case 15: logContent = "[RACE] ��ǰ����";g_time_piece_length = 10;break;
		default: cout<<"[WARNING] �����ڵĻ�������"<<endl;break;
		}
	} else {
		switch(sin) {
		case 0:logContent = "[RACE] ������������л�";g_time_piece_length = 10;break;
		default: cout<<"[WARNING] �����ڵĹ�������"<<endl;break;
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
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID;		 ///< Ŀ�ķɻ����
	outputData.navCtrlCmd	= true;			///< ��·����ָ��
	outputData.speedCtrlCmd = true;			///< �ٶȱ���ָ��
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
	outputData.desireNavAlt = 3000;    ///< ������·�߶�(m)
	outputData.desireSpeed	= 1000;     ///< ������·�ٶ�(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
	sendFlyControlCmd(outputData);
}

/// \brief ��һ�ܵл�����
void MyStrategy::DoTacToTar()
{
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

/// \brief ����ָ���߶�
void MyStrategy::DoTacAlt(double rate)
{
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

/// \brief ���λ���
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

/// \brief ��תƫ��30
void MyStrategy::DoTurnLeft30()
{
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
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
	sendFlyControlCmd(outputData);
}

/// \brief ��תƫ��60
void MyStrategy::DoTurnLeft60()
{
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
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
	sendFlyControlCmd(outputData);
}

/// \brief ��תƫ��30
void MyStrategy::DoTurnRight30()
{
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
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
	sendFlyControlCmd(outputData);
}

/// \brief ��תƫ��60
void MyStrategy::DoTurnRight60()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
	outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
	outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
	if (mACFlightStatus.heading > 0)	
		outputData.desireHead = mACFlightStatus.heading + 1.05;	///< ������·����(rad)
	else
		outputData.desireHead = mACFlightStatus.heading - 1.05;
	outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
	sendFlyControlCmd(outputData);
}

/// \brief ��ͷ
void MyStrategy::DoTacHeadEvade()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
	outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
	outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
	outputData.desireHead = mACFlightStatus.heading + PI;///< ������·����(rad)
	outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
	sendFlyControlCmd(outputData);
}

/// \brief ��ǰ����
void MyStrategy::DoTurnFor()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
	outputData.headCtrlCmd = true;
	outputData.desireHead = mACFlightStatus.heading;		///< ������·����(rad)
	outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
	sendFlyControlCmd(outputData);
}

/// \brief �ػ�
void MyStrategy::DoTacCir() {
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
	outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
	outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
	outputData.desireHead = mACFlightStatus.heading + 2 * PI;///< ������·����(rad)
	outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
	sendFlyControlCmd(outputData);
}

//--------------------------------------
// ����������
///  \brief ��������
void MyStrategy::DoTacWpnShoot()
{
	ACAI::WpnControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.launchPlaneID = mACFlightStatus.flightID;	///< ��������
	outputData.guidePlaneID	 = mACFlightStatus.flightID;	///< �Ƶ������

	outputData.mslLockTgtID = GetNearestTgt().tgtID;		///< ����Ŀ����
	
	outputData._cmdCnt	= mACFlightStatus.timeCounter;		///< ָ�����
	sendWpnControlCmd(outputData);
}
