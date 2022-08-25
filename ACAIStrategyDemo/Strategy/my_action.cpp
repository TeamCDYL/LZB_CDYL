#include "my_strategy.h"
#include "string.h"

//--------------------------------------
/// \brief 基本动作集
void MyStrategy::maneuver_i(int fin,int sin )//一级索引，二级索引
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
/// 机动动作库
/// \brief 向敌方防线飞行
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
	outputData.altCtrlCmd = true;        ///< 高度保持指令
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireAlt = 2000;       ///< 期望高度(m)
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	outputData.desireHead =mACRdrTarget.tgtInfos[0].azGeo;
	sendFlyControlCmd(outputData);
}

/// \brief +30度加速爬升
void MyStrategy::DoTacAltClimbP30()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.altCtrlCmd = true;        ///< 高度保持指令
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireAlt = 4000;       ///< 期望高度(m)
	outputData.desireHead = mACRdrTarget.tgtInfos[0].azGeo;///< 期望航路航向(rad)
	mACFlightStatus.pitch=0.52;		//俯仰角30度
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}

/// \brief +60度加速爬升
void MyStrategy::DoTacAltClimbP60()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.altCtrlCmd = true;        ///< 高度保持指令
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireAlt = 4000;       ///< 期望高度(m)
	outputData.desireHead = mACRdrTarget.tgtInfos[0].azGeo;///< 期望航路航向(rad)
	mACFlightStatus.pitch=1.04;		//俯仰角30度
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}

/// \brief -30度下潜
void MyStrategy::DoTacNoseDiveM30()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.altCtrlCmd = true;        ///< 高度保持指令
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireAlt = mPKConfig.MinFlyHeight+100;       ///< 期望高度(m)
	outputData.desireHead = mACRdrTarget.tgtInfos[0].azGeo;///< 期望航路航向(rad)
	mACFlightStatus.pitch=-0.52;		//俯仰角30度
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}

/// \brief -60度下潜
void MyStrategy::DoTacNoseDiveM60()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.altCtrlCmd = true;        ///< 高度保持指令
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireAlt = mPKConfig.MinFlyHeight+100;       ///< 期望高度(m)
	outputData.desireHead = mACRdrTarget.tgtInfos[0].azGeo;///< 期望航路航向(rad)
	mACFlightStatus.pitch=-1.04;		//俯仰角60度
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}

/// \brief 蛇形机动
void MyStrategy::DoTacStaHov()
{
	int nowCnt = mACFlightStatus.timeCounter;
	int cnt = nowCnt;
	while((nowCnt-cnt)<=50){
		MyStrategy::DoTurnLeft();			//向左飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	while(((nowCnt-cnt)<=50)){
		MyStrategy::DoTurnFor();			//向前飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	while(((nowCnt-cnt)<=50)){
		MyStrategy::DoTurnRight();			//向右飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	while(((nowCnt-cnt)<=50)){
		MyStrategy::DoTurnFor();			//向前飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	while(((nowCnt-cnt)<=50)){
		MyStrategy::DoTurnRight();			//向右飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	while(((nowCnt-cnt)<=50)){
		MyStrategy::DoTurnFor();			//向前飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	while(((nowCnt-cnt)<=50)){
		MyStrategy::DoTurnLeft();			//向左飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	while(((nowCnt-cnt)<=50)){
		MyStrategy::DoTurnFor();			//向前飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
}

/// \brief 左转偏置30
void MyStrategy::DoTurnLeft30()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.altCtrlCmd = true;        ///< 高度保持指令
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireAlt = 2000;			///< 期望高度(m)
	outputData.desireHead = mACFlightStatus.heading-0.52;	///< 期望航路航向(rad)
	static double recordHead=outputData.desireHead;	
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
	outputData.altCtrlCmd = true;        ///< 高度保持指令
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireAlt = 2000;			///< 期望高度(m)
	outputData.desireHead = mACFlightStatus.heading-1.05;	///< 期望航路航向(rad)
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
	outputData.altCtrlCmd = true;        ///< 高度保持指令
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireAlt = 2000;			///< 期望高度(m)
	outputData.desireHead = mACFlightStatus.heading+0.52;	///< 期望航路航向(rad)
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
	outputData.altCtrlCmd = true;        ///< 高度保持指令
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireAlt = 2000;			///< 期望高度(m)
	outputData.desireHead = mACFlightStatus.heading+1.05;	///< 期望航路航向(rad)
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
	outputData.altCtrlCmd = true;        ///< 高度保持指令
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireAlt = 2000;       ///< 期望高度(m)
	outputData.desireHead = mACMslWarning.threatInfos[0].azGeo + PI;///< 期望航路航向(rad)
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}

/// \brief 掉头后30度下潜
void MyStrategy::DoTurnEvad30()
{
	int nowCnt = mACFlightStatus.timeCounter;
	int cnt = nowCnt;
	while((nowCnt-cnt)<=500){
		DoTacHeadEvade();		//掉头飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	DoTacNoseDiveM30();
}

/// \brief 掉头后60度下潜
void MyStrategy::DoTurnEvad60()
{
	int nowCnt = mACFlightStatus.timeCounter;
	int cnt = nowCnt;
	while((nowCnt-cnt)<=500){
		DoTacHeadEvade();		//掉头飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	DoTacNoseDiveM60();
}

/// \brief 左转向
void MyStrategy::DoTurnLeft()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.altCtrlCmd = true;        ///< 高度保持指令
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireAlt = mACFlightStatus.heading-PI/2.0;			///< 期望高度(m)
	outputData.desireHead = mACRdrTarget.tgtInfos[0].azGeo;		///< 期望航路航向(rad)
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}

/// \brief 右转向
void MyStrategy::DoTurnRight()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.altCtrlCmd = true;        ///< 高度保持指令
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireAlt = mACFlightStatus.heading+PI/2.0;			///< 期望高度(m)
	outputData.desireHead = mACRdrTarget.tgtInfos[0].azGeo;		///< 期望航路航向(rad)
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
	outputData.altCtrlCmd = true;        ///< 高度保持指令
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireAlt = mACFlightStatus.heading;			///< 期望高度(m)
	outputData.desireHead = mACRdrTarget.tgtInfos[0].azGeo;		///< 期望航路航向(rad)
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}

/// \brief 回环
void MyStrategy::DoTacCir() {
	unsigned long nowCnt = mACFlightStatus.timeCounter;
	unsigned long cnt = nowCnt;
	while(((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnLeft();			//向左飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	while(((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnFor();			//向前飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	while(((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnLeft();			//向左飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	while(((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnFor();			//向前飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	while(((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnLeft();			//向左飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	while(((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnFor();			//向前飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	while(((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnLeft();			//向左飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	while(((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnFor();			//向前飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
}

//--------------------------------------
// 攻击动作库
///  \brief 武器发射
void MyStrategy::DoTacWpnShoot(int m)
{
	ACAI::WpnControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.launchPlaneID = mACFlightStatus.flightID; ///< 发射机编号
	outputData.guidePlaneID	 = mACFlightStatus.flightID;  ///< 制导机编号
	switch(m){
	case 0:
	outputData.mslLockTgtID = mACRdrTarget.tgtInfos[0].tgtID;break;  ///< 导弹目标编号
	case 1: 
	outputData.mslLockTgtID = mACRdrTarget.tgtInfos[1].tgtID;break;  ///< 导弹目标编号
	default: 
	outputData.mslLockTgtID = mACRdrTarget.tgtInfos[0].tgtID;break;  ///< 导弹目标编号
	}
	
	outputData._cmdCnt	= mACFlightStatus.timeCounter;       ///< 指令计数
	sendWpnControlCmd(outputData);
}

/// \brief 切换制导机并发射武器
void MyStrategy::SwitchGuideFlight() {
	// TODO:切换制导机
	ACAI::WpnControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	if (mACFlightStatus.flightTeam==ACAI::V_FLIGHT_TEAM_RED)
	{
		int array[4]={0,1,2,1};
		outputData.launchPlaneID = array[mACFlightStatus.flightID+1]; ///< 发射机编号
		outputData.guidePlaneID	 = array[mACFlightStatus.flightID+1];  ///< 制导机编号
		outputData.mslLockTgtID = mACRdrTarget.tgtInfos[0].tgtID;  ///< 导弹目标编号
		outputData._cmdCnt	= mACFlightStatus.timeCounter;       ///< 指令计数
		sendWpnControlCmd(outputData);
	}
	else{
		int arra[4]={0,0,4,3};
		outputData.launchPlaneID = arra[mACFlightStatus.flightID-3]; ///< 发射机编号
		outputData.guidePlaneID	 = arra[mACFlightStatus.flightID-3];  ///< 制导机编号
		outputData.mslLockTgtID = mACRdrTarget.tgtInfos[0].tgtID;  ///< 导弹目标编号
		outputData._cmdCnt	= mACFlightStatus.timeCounter;       ///< 指令计数
		sendWpnControlCmd(outputData);
	}
}