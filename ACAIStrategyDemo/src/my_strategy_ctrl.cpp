#include "my_strategy.h"
#include "string.h"

int g_flight_state;	//飞机存活状态
int g_cnt_state;		//导弹威胁状态
int g_enmy_state;	//敌机数量状态
int g_launch_state;	//我方发射导弹状态
int g_guide_state;	//我方制导导弹状态

void MyStrategy::onPKStart(const ACAI::PKConfig &pkConfig)
{
    memcpy(&mPKConfig, &pkConfig, sizeof(mPKConfig));
    // TODO : 此处添加用户代码响应比赛开始
	if (mACFlightStatus.flightRole == ACAI::V_FLIGHT_ROLE_LEAD) {
		FILE *tmp = fopen("state1.csv", "w"); // 清空文件内容
		fclose(tmp);
		PrintStatus("state1.csv");
	} else {
		FILE *tmp = fopen("state2.csv", "w"); // 清空文件内容
		fclose(tmp);
		PrintStatus("state2.csv");
	}
	FILE *tmp = fopen("false", "w"); // 标志对战开始
	fclose(tmp);
	remove("true");
}

void MyStrategy::onPKEnd()
{
    // TODO : 此处添加用户代码响应比赛结束
	initData();
	FILE *tmp = fopen("true", "w"); // 标志对战结束
	fclose(tmp);
	remove("false");
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
	//规则

	double HeightEdge = (mPKConfig.MaxFlyHeight - mPKConfig.MinFlyHeight) / 20.0;
	double LonEdge = (mPKConfig.RightUpLon - mPKConfig.LeftDownLon) / 20.0;
	double LatEdge = (mPKConfig.RightUpLat - mPKConfig.LeftDownLat) / 20.0;
	double HeightCorrect = (mPKConfig.MaxFlyHeight - mPKConfig.MinFlyHeight) / 10.0;
	double LonCorrect = (mPKConfig.RightUpLon - mPKConfig.LeftDownLon) / 10.0;
	double LatCorrect = (mPKConfig.RightUpLat - mPKConfig.LeftDownLat) / 10.0;
	
	ACAI::FlyControlCmd cmd;
	memset(&cmd, 0, sizeof(cmd));
	cmd.desireAlt = mACFlightStatus.alt;
	if (mACFlightStatus.alt < mPKConfig.MinFlyHeight + HeightEdge) {
		cmd.navCtrlCmd = true;
		cmd.desireNavAlt = mPKConfig.MaxFlyHeight - HeightCorrect;
	} else if (mACFlightStatus.alt > mPKConfig.MaxFlyHeight - HeightEdge) {
		cmd.desireNavAlt = mPKConfig.MinFlyHeight + HeightCorrect;
		cmd.navCtrlCmd = true;
	}

	cmd.desireNavLon = mACFlightStatus.lon;
	if (mACFlightStatus.lon > mPKConfig.RightUpLon - LonEdge) {
		cmd.desireNavLon = mPKConfig.RightUpLon - LonCorrect;
		cmd.navCtrlCmd = true;
	} else if (mACFlightStatus.lon < mPKConfig.LeftDownLon + LonEdge) {
		cmd.desireNavLat = mPKConfig.LeftDownLon + LonCorrect;
		cmd.navCtrlCmd = true;
	}

	cmd.desireNavLat = mACFlightStatus.lat;
	if (mACFlightStatus.lat > mPKConfig.RightUpLat - LatEdge) {
		cmd.desireNavLat = mPKConfig.RightUpLat - LatCorrect;
		cmd.navCtrlCmd = true;
	} else if (mACFlightStatus.lat < mPKConfig.LeftDownLat + LatEdge) {
		cmd.desireNavLat = mPKConfig.LeftDownLat + LatCorrect;
		cmd.navCtrlCmd = true;
	}
	sendFlyControlCmd(cmd);


#ifdef DEEP_LEARNING	// 深度学习时读取动作
	readAction(); 
#endif

#ifndef DEEP_LEARNING // 专家模式时执行动作
	//单机战术
	static int mslWarningStartTime = 0;
	if (mACMslWarning.mslCnt > 0) {	
		if (mACFlightStatus.timeCounter - mslWarningStartTime > 3000) {		// 导弹警告开始3秒后躲避
			readActionByPro(0, 11);
			//DoTacHeadEvade();
			strcpy(outputData.EventDes, "掉头");
			logEvent(outputData);
		}
		 else { // 在没有导弹警告时刷新导弹警告计时器
			mslWarningStartTime = mACFlightStatus.timeCounter;
		}
	}
	// 无法攻击，无需躲避时全速突进
	if (mACRdrTarget.tgtCnt >= 0 &&	mACMslWarning.mslCnt ==0) {
		readActionByPro(0, 1);
		//DoTacPointAtk();
		strcpy(outputData.EventDes, "向敌方防线突围");
		logEvent(outputData);
	}
	if (mACRdrTarget.tgtCnt > 0 && mACMslWarning.mslCnt == 0 && mACMSLInGuide.mslCnt > 0) {
		// 判断追击还是突破
		// 存在敌人时
		if (mACFCCStatus.envInfos[0].FPoleValid || mACFCCStatus.envInfos[0].APoleValid) {
			readActionByPro(1, 1);
			//DoTacWpnShoot();
			strcpy(outputData.EventDes, "武器发射");
			logEvent(outputData);
		}
		if (mACFCCStatus.envInfos[1].FPoleValid || mACFCCStatus.envInfos[1].APoleValid) {
			readActionByPro(1, 3);
			//DoTacWpnShoot(1);
			strcpy(outputData.EventDes,"武器发射");
			logEvent(outputData);
		} else {
			readActionByPro(0, 2);
			//DoTacToTar();
			strcpy(outputData.EventDes, "向一架敌方飞机飞行");
			logEvent(outputData);
		}
	}
	if (mACRdrTarget.tgtCnt > 0 && mACMslWarning.mslCnt == 0 && mACMSLInGuide.mslCnt == 0) {
		readActionByPro(0, 4);
		//DoTacAltClimbP60();
		strcpy(outputData.EventDes, "加60度上升");
		logEvent(outputData);
		if (mACFlightStatus.alt==4000) {
			readActionByPro(0, 1);
			//DoTacPointAtk();
			strcpy(outputData.EventDes, "向敌方防线突袭");
			logEvent(outputData);
		}
	}
#endif
}

//--------------------------------------
//读取动作
struct Action {
	int fin, sin; // 一级索引，0：飞行，1：攻击；二级索引：具体动作
};

void deal(const char* filename, LPVOID lParam) { // 特殊原因不方便加到MyStrategy类里
	FILE* fp = fopen(filename, "r");
	int fin, sin; // 一级索引，二级索引
	fscanf(fp, "%d %d", &fin, &sin);
	fclose(fp);
	struct Action* act = (struct Action*) lParam;
	act->fin = fin;
	act->sin = sin;
	return;
}

void MyStrategy::readAction() {
	struct Action act = {-1, -1};
	if (mACFlightStatus.flightRole == ACAI::V_FLIGHT_ROLE_LEAD) {
		if (watch(L".", "action1.csv", deal, &act)) {
			maneuver_i(act.fin, act.sin);
		} else { // 监听期间文件未发生改变
			DoTacHeadEvade();
			return;
		}
		// TODO:收到action后的反馈
		PrintStatus("state1.csv");
	} else {
		if (watch(L".", "action2.csv", deal, &act)) {
			maneuver_i(act.fin, act.sin);
		} else { // 监听期间文件未发生改变
			DoTacHeadEvade();
			return;
		}
		PrintStatus("state2.csv");
	}
	return;
}

void MyStrategy::readActionByPro(int fin, int sin) { // 一二级索引
	maneuver_i(fin, sin);
#ifndef DEEP_LEARNING // 专家模式时输出动作
	if (mACFlightStatus.flightRole == ACAI::V_FLIGHT_ROLE_LEAD) {
		FILE *fp = fopen("action1.csv", "w");
		fprintf(fp, "%d, %d", fin, sin);
		fclose(fp);
	} else {
		FILE *fp = fopen("action2.csv", "w");
		fprintf(fp, "%d, %d", fin, sin);
		fclose(fp);
	}
#endif
}

#define POW2(x) (x) * (x)

double getTdAngle(const double vect1[3], const double vect2[3]) {
	double vectDot = vect1[0] * vect2[0] + vect1[1] * vect2[1] + vect1[2] * vect2[2];
	double vectMod1 = sqrt( POW2(vect1[0]) + POW2(vect1[1]) + POW2(vect1[2]) );
	double vectMod2 = sqrt( POW2(vect2[0]) + POW2(vect2[1]) + POW2(vect2[2]) );
	double cosValue = vectDot / (vectMod1 * vectMod2);
	double rad = acos(cosValue);
	return rad;
}

void MyStrategy::PrintStatus(const char * filename) {
	static const double dis2Lons = 160000 / (mPKConfig.RightUpLon - mPKConfig.LeftDownLon);
	static const double dis2Lats = 80000 / (mPKConfig.RightUpLat - mPKConfig.LeftDownLat);
	int nCoMslCnt = 0;
	for (int i = 0; i < mCOMSLInGuide.flightMemCnt; ++i) {
		nCoMslCnt += mCOMSLInGuide.memMSLInGuide[0].mslCnt;
	}
	FILE* fp = fopen(filename, "r");
	if (fp == NULL) {
		fclose(fp);
		fp = fopen(filename, "w");
	} else {
		fclose(fp);
		fp = fopen(filename, "a");
	}
	fprintf(fp, "[%d], %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %d, %d, %d, %d, %d, %d\n",
		mACFlightStatus.timeCounter,						// [时标]
		mACRdrTarget.tgtInfos[0].slantRange,				// 目标距离
		mACFlightStatus.alt - mACRdrTarget.tgtInfos[0].alt, // 目标高度差
		mACRdrTarget.tgtInfos[0].azGeo,						// 目标 地理系 方位角
		mACRdrTarget.tgtInfos[0].sbsSpeed,					// 目标速度(m/s)
		mCOFlightStatus.memFlightStatus[0].groundSpeed,		// 友机速度(m/s)
		sqrt( POW2(mACFlightStatus.alt - mCOFlightStatus.memFlightStatus[0].alt) +
		POW2((mACFlightStatus.lon - mCOFlightStatus.memFlightStatus[0].lon) * dis2Lons) +
		POW2((mACFlightStatus.lat - mCOFlightStatus.memFlightStatus[0].lat) * dis2Lats) ),
															// 友机距离
		mACFlightStatus.alt - mCOFlightStatus.memFlightStatus[0].alt,
															// 友机高度差
		getTdAngle(mACFlightStatus.velNWU, mCOFlightStatus.memFlightStatus[0].velNWU),
															// 友机速度矢量夹角
		mACFlightStatus.groundSpeed,						// 本机速度
		getTdAngle(mACFlightStatus.velNWU, mACRdrTarget.tgtInfos[0].velNWU),
															// 敌机速度矢量夹角
		mACMslWarning.mslCnt > 0,							// 是否被导弹锁定
		mACMSLInGuide.mslCnt > 0,							// 是否在制导
		mACFlightStatus.remainWpnNum,						// 剩余武器量
		mACRdrTarget.tgtCnt,								// 扫描到的敌机数量
		mACMslWarning.mslCnt,								// 锁定自己的导弹数量
		mACMSLInGuide.mslCnt + nCoMslCnt					// 场上我方导弹数量
		);
	fclose(fp);
}

//--------------------------------------
//规则


//--------------------------------------
//基本动作集
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
//向敌方防线飞行
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
//向一架敌机飞行
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
//+30度加速爬升
void MyStrategy::DoTacAltClimbP30()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.altCtrlCmd = true;        ///< 高度保持指令
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireAlt = 4000;       ///< 期望高度(m)
	outputData.desireHead = mACRdrTarget.tgtInfos[0].azGeo+PI/6.0;///< 期望航路航向(rad)
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}
//+60度加速爬升
void MyStrategy::DoTacAltClimbP60()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.altCtrlCmd = true;        ///< 高度保持指令
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireAlt = 4000;       ///< 期望高度(m)
	outputData.desireHead = mACRdrTarget.tgtInfos[0].azGeo+PI/3.0;///< 期望航路航向(rad)
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}
//-30度下潜
void MyStrategy::DoTacNoseDiveM30()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.altCtrlCmd = true;        ///< 高度保持指令
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireAlt = mPKConfig.MinFlyHeight+100;       ///< 期望高度(m)
	outputData.desireHead = mACRdrTarget.tgtInfos[0].azGeo-PI/6.0;///< 期望航路航向(rad)
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}
//-60度下潜
void MyStrategy::DoTacNoseDiveM60()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.altCtrlCmd = true;        ///< 高度保持指令
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireAlt = mPKConfig.MinFlyHeight+100;       ///< 期望高度(m)
	outputData.desireHead = mACRdrTarget.tgtInfos[0].azGeo-PI/3.0;///< 期望航路航向(rad)
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}
//蛇形机动
void MyStrategy::DoTacStaHov()
{
	int nowCnt = mACFlightStatus.timeCounter;
	int cnt = nowCnt;
	while((nowCnt-cnt)<=500){
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
//左转偏置30
void MyStrategy::DoTurnLeft30()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.altCtrlCmd = true;        ///< 高度保持指令
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireAlt = 2000;			///< 期望高度(m)
	outputData.desireHead = mACFlightStatus.heading-PI/6.0;	///< 期望航路航向(rad)
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}
//左转偏置60
void MyStrategy::DoTurnLeft60()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.altCtrlCmd = true;        ///< 高度保持指令
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireAlt = 2000;			///< 期望高度(m)
	outputData.desireHead = mACFlightStatus.heading-PI/3.0;	///< 期望航路航向(rad)
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}
//右转偏置30
void MyStrategy::DoTurnRight30()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.altCtrlCmd = true;        ///< 高度保持指令
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireAlt = 2000;			///< 期望高度(m)
	outputData.desireHead = mACFlightStatus.heading+PI/6.0;	///< 期望航路航向(rad)
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}
//右转偏置60
void MyStrategy::DoTurnRight60()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< 目的飞机编号
	outputData.altCtrlCmd = true;        ///< 高度保持指令
	outputData.headCtrlCmd = true;       ///< 航向保持指令
	outputData.speedCtrlCmd = true;      ///< 速度保持指令
	outputData.desireAlt = 2000;			///< 期望高度(m)
	outputData.desireHead = mACFlightStatus.heading+PI/3.0;	///< 期望航路航向(rad)
	outputData.desireSpeed = 1000;///< 期望航路速度(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< 指令计数
	sendFlyControlCmd(outputData);
}
//掉头
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

//掉头后30度下潜
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
//掉头后60度下潜
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
//-30度下潜
//左转向
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
//右转向
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
//向前飞行
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

//--------------------------------------
//战术动作库
// 武器发射
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

//切换制导机并发射武器
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
//回环
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
//战术动作库
// 追击
void MyStrategy::DoTrack() {
	MyStrategy::DoTacToTar();
}

// 偏置制导
/*void MyStrategy::DoTacHeadGuide()
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
}*/
//状态设置
void MyStrategy::SetFlightState(unsigned int flightID)
{
	// 判断友方（或者自身）飞机被击落的情况，返回状态值2
	if (flightID == 3 && mACFlightStatus.flightID == 4)
		g_flight_state = 2;
	if(flightID == 4 && mACFlightStatus.flightID == 3)
		g_flight_state = 2;
	if(flightID == 1 && mACFlightStatus.flightID == 2)
		g_flight_state = 2;
	if(flightID == 2 && mACFlightStatus.flightID == 1)
		g_flight_state = 2;
	if(flightID == mACFlightStatus.flightID)
		g_flight_state = 2;

	// 判断敌方飞机被击落的情况，返回状态值1
	if (flightID == 3 && mACFlightStatus.flightID == 1)
		g_flight_state = 1;
	if (flightID == 3 && mACFlightStatus.flightID == 2)
		g_flight_state = 1;
	if (flightID == 4 && mACFlightStatus.flightID == 1)
		g_flight_state = 1;
	if (flightID == 4 && mACFlightStatus.flightID == 2)
		g_flight_state = 1;
	if (flightID == 1 && mACFlightStatus.flightID == 3)
		g_flight_state = 1;
	if (flightID == 1 && mACFlightStatus.flightID == 4)
		g_flight_state = 1;
	if (flightID == 2 && mACFlightStatus.flightID == 3)
		g_flight_state = 1;
	if	(flightID == 2 && mACFlightStatus.flightID == 4)
		g_flight_state = 1;
}
//奖赏函数
int MyStrategy::OutputReward()
{
	int reward = 0;

	// 敌方飞机被击落的情况（击落一架敌机）
	if(g_flight_state == 1)
	{
		// 重置状态变量
		g_flight_state = 0;
		reward += 250;
	}
	// 友方飞机或自身被击落的情况（友方飞机被击落）
	if(g_flight_state == 2)
	{
		// 重置状态变量
		g_flight_state = 0;
		reward -= 250;
	}
	// 当导弹威胁数增加且没有飞机被击中时的reward（我方被敌方导弹锁定）
	if (g_cnt_state < mACMslWarning.mslCnt && g_flight_state == 0 && mACFlightStatus.flightID == mACMslWarning.flightID)
	{
		g_cnt_state++;
		reward -= 5;
	}
	// 当导弹威胁数减少并且没有飞机坠毁时的reward（我方逃脱敌方导弹）
	if (g_cnt_state > mACMslWarning.mslCnt && g_flight_state == 0 && mACFlightStatus.flightID == mACMslWarning.flightID)
	{
		g_cnt_state--;
		reward += 10;
	}
	// 新侦查到敌机的情况（我方获得敌方视野）
	if (g_enmy_state < mACRdrTarget.tgtCnt && g_flight_state == 0)
	{
		g_enmy_state++;
		reward += 2;
	}
	// 侦察到的敌机数量减少（我方丢失敌方视野）
	if (g_enmy_state > mACRdrTarget.tgtCnt && g_flight_state == 0)
	{
		g_enmy_state--;
		reward -= 2;
	}
	// 飞机发射导弹
	if (g_launch_state < mTeamMSLLaunched.mslCnt && g_flight_state == 0 && mTeamMSLLaunched.trajectoryInfos[0].launchFlightID == mACFlightStatus.flightID)
	{
		g_launch_state++;
		reward -= 10;
	}
	// 我方导弹锁定敌方一架飞机
	if (g_guide_state < mACMSLInGuide.mslCnt && g_flight_state == 0 && mACMSLInGuide.guideFlightID == mACFlightStatus.flightID)
	{
		g_guide_state++;
		reward += 5;
	}
	return reward;
}
//打印出奖赏值
void MyStrategy::PrintReward() {
	int numberReward;
	FILE* fp;
	if((fp=fopen("reward.csv","a"))==NULL)
	{
		printf("failed open file");
	} else {
		numberReward = OutputReward();
		fprintf(fp,"%d\n",numberReward);
	}
	fclose(fp);
}
