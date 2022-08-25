#include "my_strategy.h"
#include "string.h"

//--------------------------------------
/// \brief ����������
void MyStrategy::maneuver_i(int fin,int sin )//һ����������������
{
	if (fin == 0) {
		switch(sin) {
		case 1: DoTacPointAtk();break;
		case 2: DoTacToTar();break;
		case 3:	DoTacAltClimbP30();break;
		case 4: DoTacAltClimbP60();break;
		case 5: DoTacNoseDiveM30();break;
		case 6:	DoTacNoseDiveM60();break;
		case 7: DoTacStaHov();break;
		case 8: DoTurnLeft30();break;
		case 9: DoTurnLeft60();break;
		case 10: DoTurnRight30();break;
		case 11: DoTacHeadEvade();break;
		case 12: DoTurnEvad30();break;
		case 13: DoTurnEvad60();break;
		default: DoTacHeadEvade();break;
		}
	} else {
		switch(sin) {
		case 1:DoTacWpnShoot();break;
		case 2:SwitchGuideFlight();break;
		case 3:DoTacWpnShoot(1);break;
		default:break;
		}
	}
}

//--------------------------------------
/// ����������
/// \brief ��з����߷���
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
	outputData.altCtrlCmd = true;        ///< �߶ȱ���ָ��
	outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
	outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
	outputData.desireAlt = 2000;       ///< �����߶�(m)
	outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
	outputData.desireHead =mACRdrTarget.tgtInfos[0].azGeo;
	sendFlyControlCmd(outputData);
}

/// \brief +30�ȼ�������
void MyStrategy::DoTacAltClimbP30()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
	outputData.altCtrlCmd = true;        ///< �߶ȱ���ָ��
	outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
	outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
	outputData.desireAlt = 4000;       ///< �����߶�(m)
	outputData.desireHead = mACRdrTarget.tgtInfos[0].azGeo;///< ������·����(rad)
	mACFlightStatus.pitch=0.52;		//������30��
	outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
	sendFlyControlCmd(outputData);
}

/// \brief +60�ȼ�������
void MyStrategy::DoTacAltClimbP60()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
	outputData.altCtrlCmd = true;        ///< �߶ȱ���ָ��
	outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
	outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
	outputData.desireAlt = 4000;       ///< �����߶�(m)
	outputData.desireHead = mACRdrTarget.tgtInfos[0].azGeo;///< ������·����(rad)
	mACFlightStatus.pitch=1.04;		//������30��
	outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
	sendFlyControlCmd(outputData);
}

/// \brief -30����Ǳ
void MyStrategy::DoTacNoseDiveM30()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
	outputData.altCtrlCmd = true;        ///< �߶ȱ���ָ��
	outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
	outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
	outputData.desireAlt = mPKConfig.MinFlyHeight+100;       ///< �����߶�(m)
	outputData.desireHead = mACRdrTarget.tgtInfos[0].azGeo;///< ������·����(rad)
	mACFlightStatus.pitch=-0.52;		//������30��
	outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
	sendFlyControlCmd(outputData);
}

/// \brief -60����Ǳ
void MyStrategy::DoTacNoseDiveM60()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
	outputData.altCtrlCmd = true;        ///< �߶ȱ���ָ��
	outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
	outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
	outputData.desireAlt = mPKConfig.MinFlyHeight+100;       ///< �����߶�(m)
	outputData.desireHead = mACRdrTarget.tgtInfos[0].azGeo;///< ������·����(rad)
	mACFlightStatus.pitch=-1.04;		//������60��
	outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
	sendFlyControlCmd(outputData);
}

/// \brief ���λ���
void MyStrategy::DoTacStaHov()
{
	int nowCnt = mACFlightStatus.timeCounter;
	int cnt = nowCnt;
	while((nowCnt-cnt)<=50){
		MyStrategy::DoTurnLeft();			//�������
		nowCnt=mACFlightStatus.timeCounter;	//����ʱ��	
	}
	while(((nowCnt-cnt)<=50)){
		MyStrategy::DoTurnFor();			//��ǰ����
		nowCnt=mACFlightStatus.timeCounter;	//����ʱ��	
	}
	while(((nowCnt-cnt)<=50)){
		MyStrategy::DoTurnRight();			//���ҷ���
		nowCnt=mACFlightStatus.timeCounter;	//����ʱ��	
	}
	while(((nowCnt-cnt)<=50)){
		MyStrategy::DoTurnFor();			//��ǰ����
		nowCnt=mACFlightStatus.timeCounter;	//����ʱ��	
	}
	while(((nowCnt-cnt)<=50)){
		MyStrategy::DoTurnRight();			//���ҷ���
		nowCnt=mACFlightStatus.timeCounter;	//����ʱ��	
	}
	while(((nowCnt-cnt)<=50)){
		MyStrategy::DoTurnFor();			//��ǰ����
		nowCnt=mACFlightStatus.timeCounter;	//����ʱ��	
	}
	while(((nowCnt-cnt)<=50)){
		MyStrategy::DoTurnLeft();			//�������
		nowCnt=mACFlightStatus.timeCounter;	//����ʱ��	
	}
	while(((nowCnt-cnt)<=50)){
		MyStrategy::DoTurnFor();			//��ǰ����
		nowCnt=mACFlightStatus.timeCounter;	//����ʱ��	
	}
}

/// \brief ��תƫ��30
void MyStrategy::DoTurnLeft30()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
	outputData.altCtrlCmd = true;        ///< �߶ȱ���ָ��
	outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
	outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
	outputData.desireAlt = 2000;			///< �����߶�(m)
	outputData.desireHead = mACFlightStatus.heading-0.52;	///< ������·����(rad)
	static double recordHead=outputData.desireHead;	
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
	outputData.altCtrlCmd = true;        ///< �߶ȱ���ָ��
	outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
	outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
	outputData.desireAlt = 2000;			///< �����߶�(m)
	outputData.desireHead = mACFlightStatus.heading-1.05;	///< ������·����(rad)
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
	outputData.altCtrlCmd = true;        ///< �߶ȱ���ָ��
	outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
	outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
	outputData.desireAlt = 2000;			///< �����߶�(m)
	outputData.desireHead = mACFlightStatus.heading+0.52;	///< ������·����(rad)
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
	outputData.altCtrlCmd = true;        ///< �߶ȱ���ָ��
	outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
	outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
	outputData.desireAlt = 2000;			///< �����߶�(m)
	outputData.desireHead = mACFlightStatus.heading+1.05;	///< ������·����(rad)
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
	outputData.altCtrlCmd = true;        ///< �߶ȱ���ָ��
	outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
	outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
	outputData.desireAlt = 2000;       ///< �����߶�(m)
	outputData.desireHead = mACMslWarning.threatInfos[0].azGeo + PI;///< ������·����(rad)
	outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
	sendFlyControlCmd(outputData);
}

/// \brief ��ͷ��30����Ǳ
void MyStrategy::DoTurnEvad30()
{
	int nowCnt = mACFlightStatus.timeCounter;
	int cnt = nowCnt;
	while((nowCnt-cnt)<=500){
		DoTacHeadEvade();		//��ͷ����
		nowCnt=mACFlightStatus.timeCounter;	//����ʱ��	
	}
	DoTacNoseDiveM30();
}

/// \brief ��ͷ��60����Ǳ
void MyStrategy::DoTurnEvad60()
{
	int nowCnt = mACFlightStatus.timeCounter;
	int cnt = nowCnt;
	while((nowCnt-cnt)<=500){
		DoTacHeadEvade();		//��ͷ����
		nowCnt=mACFlightStatus.timeCounter;	//����ʱ��	
	}
	DoTacNoseDiveM60();
}

/// \brief ��ת��
void MyStrategy::DoTurnLeft()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
	outputData.altCtrlCmd = true;        ///< �߶ȱ���ָ��
	outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
	outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
	outputData.desireAlt = mACFlightStatus.heading-PI/2.0;			///< �����߶�(m)
	outputData.desireHead = mACRdrTarget.tgtInfos[0].azGeo;		///< ������·����(rad)
	outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
	sendFlyControlCmd(outputData);
}

/// \brief ��ת��
void MyStrategy::DoTurnRight()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
	outputData.altCtrlCmd = true;        ///< �߶ȱ���ָ��
	outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
	outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
	outputData.desireAlt = mACFlightStatus.heading+PI/2.0;			///< �����߶�(m)
	outputData.desireHead = mACRdrTarget.tgtInfos[0].azGeo;		///< ������·����(rad)
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
	outputData.altCtrlCmd = true;        ///< �߶ȱ���ָ��
	outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
	outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
	outputData.desireAlt = mACFlightStatus.heading;			///< �����߶�(m)
	outputData.desireHead = mACRdrTarget.tgtInfos[0].azGeo;		///< ������·����(rad)
	outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
	sendFlyControlCmd(outputData);
}

/// \brief �ػ�
void MyStrategy::DoTacCir() {
	unsigned long nowCnt = mACFlightStatus.timeCounter;
	unsigned long cnt = nowCnt;
	while(((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnLeft();			//�������
		nowCnt=mACFlightStatus.timeCounter;	//����ʱ��	
	}
	while(((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnFor();			//��ǰ����
		nowCnt=mACFlightStatus.timeCounter;	//����ʱ��	
	}
	while(((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnLeft();			//�������
		nowCnt=mACFlightStatus.timeCounter;	//����ʱ��	
	}
	while(((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnFor();			//��ǰ����
		nowCnt=mACFlightStatus.timeCounter;	//����ʱ��	
	}
	while(((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnLeft();			//�������
		nowCnt=mACFlightStatus.timeCounter;	//����ʱ��	
	}
	while(((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnFor();			//��ǰ����
		nowCnt=mACFlightStatus.timeCounter;	//����ʱ��	
	}
	while(((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnLeft();			//�������
		nowCnt=mACFlightStatus.timeCounter;	//����ʱ��	
	}
	while(((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnFor();			//��ǰ����
		nowCnt=mACFlightStatus.timeCounter;	//����ʱ��	
	}
}

//--------------------------------------
// ����������
///  \brief ��������
void MyStrategy::DoTacWpnShoot(int m)
{
	ACAI::WpnControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.launchPlaneID = mACFlightStatus.flightID; ///< ��������
	outputData.guidePlaneID	 = mACFlightStatus.flightID;  ///< �Ƶ������
	switch(m){
	case 0:
	outputData.mslLockTgtID = mACRdrTarget.tgtInfos[0].tgtID;break;  ///< ����Ŀ����
	case 1: 
	outputData.mslLockTgtID = mACRdrTarget.tgtInfos[1].tgtID;break;  ///< ����Ŀ����
	default: 
	outputData.mslLockTgtID = mACRdrTarget.tgtInfos[0].tgtID;break;  ///< ����Ŀ����
	}
	
	outputData._cmdCnt	= mACFlightStatus.timeCounter;       ///< ָ�����
	sendWpnControlCmd(outputData);
}

/// \brief �л��Ƶ�������������
void MyStrategy::SwitchGuideFlight() {
	// TODO:�л��Ƶ���
	ACAI::WpnControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	if (mACFlightStatus.flightTeam==ACAI::V_FLIGHT_TEAM_RED)
	{
		int array[4]={0,1,2,1};
		outputData.launchPlaneID = array[mACFlightStatus.flightID+1]; ///< ��������
		outputData.guidePlaneID	 = array[mACFlightStatus.flightID+1];  ///< �Ƶ������
		outputData.mslLockTgtID = mACRdrTarget.tgtInfos[0].tgtID;  ///< ����Ŀ����
		outputData._cmdCnt	= mACFlightStatus.timeCounter;       ///< ָ�����
		sendWpnControlCmd(outputData);
	}
	else{
		int arra[4]={0,0,4,3};
		outputData.launchPlaneID = arra[mACFlightStatus.flightID-3]; ///< ��������
		outputData.guidePlaneID	 = arra[mACFlightStatus.flightID-3];  ///< �Ƶ������
		outputData.mslLockTgtID = mACRdrTarget.tgtInfos[0].tgtID;  ///< ����Ŀ����
		outputData._cmdCnt	= mACFlightStatus.timeCounter;       ///< ָ�����
		sendWpnControlCmd(outputData);
	}
}