#include "my_strategy.h"
#include "string.h"

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

bool inRange(double angle, double range) {
	return (angle <= range && angle >= -range) ||
		(PI - angle <= range && PI - angle >= -range);
}

void MyStrategy::timeSlice40()
{
    // TODO : 此处添加用户代码执行40ms周期任务
	ACAI::EventLog outputData;
	memset(&outputData, 0, sizeof(outputData));
	/*
	if( mACMslWarning.mslCnt > 0 )
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
	// 原战术
	int check = Rule();
	if (check != MyStrategy::NoWarn) {
		ACAI::FlyControlCmd cmd;
		memset(&cmd, 0, sizeof(cmd));
		cmd.altCtrlCmd = true;
		if (check & MyStrategy::AltWarn) {
			if (mACFlightStatus.alt >= mPKConfig.MaxFlyHeight - 200) {
				cmd.desireAlt = mPKConfig.MaxFlyHeight - 1000;
			} else {
				cmd.desireAlt = mPKConfig.MinFlyHeight + 1000;
			}
		} 

		if (check & MyStrategy::LonWarn) {
			if (mACFlightStatus.lon >= mPKConfig.RightUpLon - 5) {
				cmd.desireNavLon = mPKConfig.RightUpLon - 10;
			} else {
				cmd.desireNavLon = mPKConfig.LeftDownLon + 10;
			}
		}

		if (check & MyStrategy::LatWarn) {
			if (mACFlightStatus.lat >= mPKConfig.RightUpLat - 5) {
				cmd.desireNavLat = mPKConfig.RightUpLat - 10;
			} else {
				cmd.desireNavLat = mPKConfig.LeftDownLat + 10;
			}
		}
		sendFlyControlCmd(cmd);
	}

	//单机战术
//	if (mCOFlightStatus.flightMemCnt == 0) {
		if (mACMslWarning.mslCnt > 0) {
			if (mACFlightStatus.timeCounter - mslWarningStartTime > 3000) { // 导弹警告开始3秒后躲避
				DoTacHeadEvade();
				strcpy(outputData.EventDes, "置尾逃逸");
				logEvent(outputData);
			}
		} else { // 在没有导弹警告时刷新导弹警告计时器
			mslWarningStartTime = mACFlightStatus.timeCounter;
		}
		// 无法攻击，无需躲避时全速突进
		if (mACRdrTarget.tgtCnt == 0 &&	mACMslWarning.mslCnt ==0) {
			//DoTacPointAtk();
			DoTacAltClimb();
			strcpy(outputData.EventDes, "定向突防");
			logEvent(outputData);
		}
		if (mACRdrTarget.tgtCnt > 0 && mACMslWarning.mslCnt == 0 && mACMSLInGuide.mslCnt == 0) {
			// 判断追击还是突破
			// 存在敌人时
			if ((mACFCCStatus.envInfos[0].FPoleValid || mACFCCStatus.envInfos[0].APoleValid) &&
				((int) mACFlightStatus.timeCounter - (int) m_lastWpnShootTimeCounter > 5000)) {
					m_lastWpnShootTimeCounter = mACFlightStatus.timeCounter;
					DoTacWpnShoot(0);
					strcpy(outputData.EventDes, "武器发射");
					logEvent(outputData);
			} else {
				DoTacToTar(0);
				strcpy(outputData.EventDes, "追击");
				logEvent(outputData);
			}
			// 有两个敌人时
			if (mACRdrTarget.tgtCnt == 2) {
				if (mACFCCStatus.envInfos[1].FPoleValid == true &&
					(int) mACFlightStatus.timeCounter - (int) m_lastWpnShootTimeCounter > 5000) {
					m_lastWpnShootTimeCounter = mACFlightStatus.timeCounter;
					DoTacWpnShoot(1);
					strcpy(outputData.EventDes, "武器发射");
					logEvent(outputData);
				}
			}
		}
		if (mACRdrTarget.tgtCnt > 0 && mACMslWarning.mslCnt == 0 && mACMSLInGuide.mslCnt > 0) {
			DoTacHeadGuide();
			strcpy(outputData.EventDes, "偏置制导");
			logEvent(outputData);
		}
//	} else {

//	}
}

//--------------------------------------
//规则
int MyStrategy::Rule()
{
	int flag = MyStrategy::NoWarn;
	if(mACFlightStatus.alt < mPKConfig.MinFlyHeight + 200 ||
		mACFlightStatus.alt > mPKConfig.MaxFlyHeight - 200)
		flag |= MyStrategy::AltWarn;
	if(mACFlightStatus.lon > mPKConfig.RightUpLon - 5 ||
		mACFlightStatus.lon < mPKConfig.LeftDownLon + 5)
		flag |= MyStrategy::LonWarn;
	if(mACFlightStatus.lat > mPKConfig.RightUpLat - 5 ||
		mACFlightStatus.lat < mPKConfig.LeftDownLat + 5)
		flag |= MyStrategy::LatWarn;
	return flag;
}

//--------------------------------------
//基本动作集

//转向目标
void MyStrategy::DoTacToTar(int target)
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.altCtrlCmd = true;        ///< 高度保持指令
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireAlt = mACRdrTarget.tgtInfos[target].alt + 1500;       ///< 期望高度(m)
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	outputData.desireHead =mACRdrTarget.tgtInfos[target].azGeo;
	sendFlyControlCmd(outputData);
}

//下落俯冲飞行
void MyStrategy::DoTacNoseDive()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.altCtrlCmd = true;        ///< 高度保持指令
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireAlt = mPKConfig.MinFlyHeight+100;       ///< 期望高度(m)
	outputData.desireHead = mACRdrTarget.tgtInfos[0].azGeo;///< 期望航路航向(rad)
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}

//左转向
void MyStrategy::DoTurnLeft()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireHead = mACFlightStatus.heading-PI/2.0;		///< 期望航路航向(rad)
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}

//右转向
void MyStrategy::DoTurnRight()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireHead = mACFlightStatus.heading+PI/2.0;		///< 期望航路航向(rad)
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}

//向前飞行
void MyStrategy::DoTurnFor()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireHead = mACRdrTarget.tgtInfos[0].azGeo;		///< 期望航路航向(rad)
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}

//--------------------------------------
//战术动作库
// 追击
void MyStrategy::DoTrack(int target) {
	DoTacToTar(target);
}

//回环
void MyStrategy::DoTacCir() {
	unsigned long nowCnt = mACFlightStatus.timeCounter;
	unsigned long cnt = nowCnt;
	while(Rule()&&((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnLeft();			//向左飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	while(Rule()&&((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnFor();			//向前飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	while(Rule()&&((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnLeft();			//向左飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	while(Rule()&&((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnFor();			//向前飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	while(Rule()&&((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnLeft();			//向左飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	while(Rule()&&((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnFor();			//向前飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	while(Rule()&&((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnLeft();			//向左飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	while(Rule()&&((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnFor();			//向前飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
}

//蛇形机动
void MyStrategy::DoTacStaHov()
{
	int nowCnt = mACFlightStatus.timeCounter;
	int cnt = nowCnt;
	while(Rule()&&((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnLeft();			//向左飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	while(Rule()&&((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnFor();			//向前飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	while(Rule()&&((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnRight();			//向右飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	while(Rule()&&((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnFor();			//向前飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	while(Rule()&&((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnRight();			//向右飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	while(Rule()&&((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnFor();			//向前飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	while(Rule()&&((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnLeft();			//向左飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
	while(Rule()&&((nowCnt-cnt)<=5)){
		MyStrategy::DoTurnFor();			//向前飞行
		nowCnt=mACFlightStatus.timeCounter;	//现在时间	
	}
}

// 武器发射
void MyStrategy::DoTacWpnShoot(int target)
{
	ACAI::WpnControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.launchPlaneID = mACFlightStatus.flightID; ///< 发射机编号
    outputData.guidePlaneID	 = mACFlightStatus.flightID;  ///< 制导机编号
    outputData.mslLockTgtID = mACRdrTarget.tgtInfos[target].tgtID;  ///< 导弹目标编号
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
    outputData.desireNavAlt = 4000;		///< 期望航路高度(m)
    outputData.desireSpeed	= 1000;		///< 期望航路速度(m/s)
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
    outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
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
    outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
    outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}

// 爬升提速
void MyStrategy::DoTacAltClimb()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
    outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.navCtrlCmd = true;
    outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
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
	outputData.desireNavAlt = mACFlightStatus.alt + 100;
    outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}
