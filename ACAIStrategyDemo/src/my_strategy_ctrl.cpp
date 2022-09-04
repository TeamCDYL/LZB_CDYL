// TODO 1.极限情况动作覆盖 4.python训练协调

#include "my_strategy.h"
#include "string.h"
#include <iostream>
using namespace std;

#define LEAD_STATE "state1.csv"
#define WING_STATE "state2.csv"
#define LEAD_ACTION "action1.csv"
#define WING_ACTION "action2.csv"
#define MAX_DIS 50000
#define POW2(x) (x) * (x)

const char * StateTitle = "[time counter], distance to enemy, alt distance to enemy, "
						"direction to enemy, enemy speed, friend speed, "
						"distance to friend, alt distance to friend, speed direct to friend, "
						"self speed, speed direct to enemy, is locked by msl, "
						"is in guide, remaining msl, enemy in rdr, "
						"n msl locked, our n msl, reward\n";
const char * ActionTitle = "action first index, action second index\n";

int timecounter = 0;

unsigned int g_flight_state;	///< 飞机存活状态
unsigned int g_cnt_state;	///< 导弹威胁状态
unsigned int g_enmy_state;	///< 敌机数量状态
unsigned int g_launch_state;	///< 我方发射导弹状态
unsigned int g_win_state;
bool g_fight_init;
int last_tgt_num;
int tgt_num;

/// \brief 比赛开始时执行
void MyStrategy::onPKStart(const ACAI::PKConfig &pkConfig)
{
	g_flight_state = 0;	///< 飞机存活状态
	g_cnt_state = 0;	///< 导弹威胁状态
	g_enmy_state = 0;	///< 敌机数量状态
	g_launch_state = 6;	///< 我方发射导弹状态
	g_win_state = 0;

	initData(1);
    memcpy(&mPKConfig, &pkConfig, sizeof(mPKConfig));

	remove("start");
	remove("end");
	remove("fight");
	remove("outft");
	FILE *tmp_1 = fopen("start", "w");
	fclose(tmp_1);
	FILE *tmp_2 = fopen("outft", "w");
	fclose(tmp_2);

	g_fight_init = false;
	last_tgt_num = tgt_num = GetTgtSum();

	remove(LEAD_STATE);
	remove(WING_STATE);
	remove(LEAD_ACTION);
	remove(WING_ACTION);

	if (flightRole == 1) {
		FILE *tmp = fopen(LEAD_STATE, "w"); // 清空文件内容
		fprintf(tmp, StateTitle);			// 文件表头
		fclose(tmp);
		tmp = fopen(LEAD_ACTION, "w");
		fprintf(tmp, ActionTitle);
		fclose(tmp);
	} else {
		FILE *tmp = fopen(WING_STATE, "w"); // 清空文件内容
		fprintf(tmp, StateTitle);			// 文件表头
		fclose(tmp);
		tmp = fopen(WING_ACTION, "w");
		fprintf(tmp, ActionTitle);
		fclose(tmp);
	}
}

/// \brief 比赛结束时执行 
void MyStrategy::onPKEnd()
{
	if (!judge())
		g_win_state == 2;
	else
		g_win_state == 1;
	PrintState();
	remove("start");
	remove("end");
	remove("fight");
	remove("outft");
	FILE *tmp = fopen("end", "w"); // 标志对战结束
	fclose(tmp);
	g_fight_init = false;
	initData(1);
	if (!judge())
		g_win_state == 2;
	else
		g_win_state == 1;
}

void MyStrategy::onPKOut(unsigned int flightID)
{
	SetFlightState(flightID);
}

void MyStrategy::backGround()
{

}

void MyStrategy::timeSlice20()
{

}

bool inRange(double angle, double range) {
	return (angle <= range && angle >= -range) ||
		(PI - angle <= range && PI - angle >= -range);
}

/// \brief 40ms线程
void MyStrategy::timeSlice40()
{
	last_tgt_num = tgt_num;
	tgt_num = GetTgtSum();
	if (tgt_num > 0) {
		FILE *tmp = fopen("fight", "w"); // 标志进入作战状态
		fclose(tmp);
		remove("outft");
	}
	else {
		FILE *tmp = fopen("outft", "w"); // 标志进入脱战状态
		fclose(tmp);
		remove("fight");
	}

	if ((last_tgt_num > 0 && !(tgt_num > 0)))
		PrintState();

	// 第一次进入作战状态的初始化
	if (tgt_num > 0 && g_fight_init == false) {
		g_fight_init = true;
		PrintState();
	}

	if (action_finished && g_fight_init)
	{
#ifdef DEEP_LEARNING	// 深度学习时读取动作
		static bool isReadAction = false;
		if (isReadAction) PrintState();
		isReadAction = readAction();
#endif

#ifndef DEEP_LEARNING // 专家模式时执行动作
		//单机战术
		static int mslWarningStartTime = 0;
		if (mACMslWarning.mslCnt > 0) {	
			if (mACFlightStatus.timeCounter - mslWarningStartTime > 3000) {		// 导弹警告开始3秒后躲避
				readActionByPro(0, 11);
				//DoTacHeadEvade();
				if (GetTgtSum() > 0){
					PrintState();
					PrintReward();
				}
				strcpy(outputData.EventDes, "掉头");
				logEvent(outputData);
			}
			else { // 在没有导弹警告时刷新导弹警告计时器
				mslWarningStartTime = mACFlightStatus.timeCounter;
			}
		}
		// 无法攻击，无需躲避时全速突进
		if (mACRdrTarget.tgtCnt == 0 &&	mACMslWarning.mslCnt ==0) {
			readActionByPro(0, 1);
			//DoTacPointAtk();
				if (GetTgtSum() > 0){
					PrintState();
					PrintReward();
				}
			strcpy(outputData.EventDes, "向敌方防线突围");
			logEvent(outputData);
		}
		if (mACRdrTarget.tgtCnt > 0 && mACMslWarning.mslCnt == 0 && mACMSLInGuide.mslCnt > 0) {
			// 判断追击还是突破
			// 存在敌人时
			if (mACFCCStatus.envInfos[0].FPoleValid || mACFCCStatus.envInfos[0].APoleValid) {
				readActionByPro(1, 1);
				//DoTacWpnShoot();
				if (GetTgtSum() > 0){
					PrintState();
					PrintReward();
				}
				strcpy(outputData.EventDes, "武器发射");
				logEvent(outputData);
			}
			if (mACFCCStatus.envInfos[1].FPoleValid || mACFCCStatus.envInfos[1].APoleValid) {
				readActionByPro(1, 3);
				//DoTacWpnShoot(1);
				if (GetTgtSum() > 0){
					PrintState();
					PrintReward();
				}
				strcpy(outputData.EventDes,"武器发射");
				logEvent(outputData);
			} else {
				readActionByPro(0, 2);
				//DoTacToTar();
				if (GetTgtSum() > 0){
					PrintState();
					PrintReward();
				}
				strcpy(outputData.EventDes, "向一架敌方飞机飞行");
				logEvent(outputData);
			}
		}
		if (mACRdrTarget.tgtCnt > 0 && mACMslWarning.mslCnt == 0 && mACMSLInGuide.mslCnt == 0) {
			readActionByPro(0, 4);
			//DoTacAltClimbP60();
			PrintState();
			PrintReward();
			strcpy(outputData.EventDes, "加60度上升");
			logEvent(outputData);
			if (mACFlightStatus.alt==4000) {
				readActionByPro(0, 1);
				//DoTacPointAtk();
				if (GetTgtSum() > 0){
					PrintState();
					PrintReward();
				}
				strcpy(outputData.EventDes, "向敌方防线突袭");
				logEvent(outputData);
			}
		}
#endif
	} 

	if (tgt_num == 0)
		maneuver_i(1, 3);
	else
		maneuver_i(mCmd.fin, mCmd.sin);

	//规则
	ACAI::FlyControlCmd cmd;
	memset(&cmd, 0, sizeof(cmd));
	bool rule = false;
	cmd.desireAlt = mACFlightStatus.alt;
	if (mACFlightStatus.alt < mPKConfig.MinFlyHeight + HeightEdge) {
		cmd.altCtrlCmd = true;
		cmd.desireNavAlt = mPKConfig.MaxFlyHeight - HeightCorrect;
		rule = true;
	} else if (mACFlightStatus.alt > mPKConfig.MaxFlyHeight - HeightEdge) {
		cmd.desireNavAlt = mPKConfig.MinFlyHeight + HeightCorrect;
		cmd.altCtrlCmd = true;
		rule = true;
	}

	cmd.desireNavLon = mACFlightStatus.lon;
	if (mACFlightStatus.lon > mPKConfig.RightUpLon - LonEdge) {
		cmd.desireNavLon = mPKConfig.RightUpLon - LonCorrect;
		cmd.navCtrlCmd = true;
		rule = true;
	} else if (mACFlightStatus.lon < mPKConfig.LeftDownLon + LonEdge) {
		cmd.desireNavLat = mPKConfig.LeftDownLon + LonCorrect;
		cmd.navCtrlCmd = true;
		rule = true;
	}

	cmd.desireNavLat = mACFlightStatus.lat;
	if (mACFlightStatus.lat > mPKConfig.RightUpLat - LatEdge) {
		cmd.desireNavLat = mPKConfig.RightUpLat - LatCorrect;
		cmd.navCtrlCmd = true;
		rule = true;
	} else if (mACFlightStatus.lat < mPKConfig.LeftDownLat + LatEdge) {
		cmd.desireNavLat = mPKConfig.LeftDownLat + LatCorrect;
		cmd.navCtrlCmd = true;
		rule = true;
	}
	if (rule) {
		sendFlyControlCmd(cmd);
	}
}

//--------------------------------------
/// \brief 动作处理
struct Action {
	int fin, sin; // 一级索引，0：飞行，1：攻击；二级索引：具体动作
};

bool first_read = true;
void deal(const char* filename, LPVOID lParam) { // 特殊原因不方便加到MyStrategy类里
	if (first_read) {
		first_read = false;
		return;
	}
	FILE* fp = fopen(filename, "r");
	int fin, sin; // 一级索引，二级索引
	fseek(fp, -6, SEEK_END);
	fscanf(fp, "%d,%d", &fin, &sin);
	fclose(fp);
	struct Action* act = (struct Action*) lParam;
	act->fin = fin;
	act->sin = sin;
	return;
}

bool MyStrategy::readAction()
{
	struct Action act = {-1, -1};
	if (flightRole == 1) {
		if (watch(LEAD_ACTION, deal, &act)) {
			write_maneuver(act.fin, act.sin);
			return true;
		} else { // 监听期间文件未发生改变
			//DoTacPointAtk(false);
			return false;
		}
	} else {
		if (watch(WING_ACTION, deal, &act)) {
			write_maneuver(act.fin, act.sin);
			return true;
		} else { // 监听期间文件未发生改变
			//DoTacPointAtk(false);
			return false;
		}
	}
	return false;
}

void MyStrategy::readActionByPro(int fin, int sin) { // 一二级索引
	maneuver_i(fin, sin);
#ifndef DEEP_LEARNING // 专家模式时输出动作
	// 作战情况下
	if (GetTgtSum() > 0) {
		if (mACFlightStatus.flightRole == ACAI::V_FLIGHT_ROLE_LEAD) {
		FILE *fp = fopen(LEAD_ACTION, "a");
		fprintf(fp, "%d, %d\n", fin, sin);
		fclose(fp);
		} 
		else {
		FILE *fp = fopen(WING_ACTION, "a");
		fprintf(fp, "%d, %d\n", fin, sin);
		fclose(fp);
		}
	}
#endif // DEEP_LEARNING
}

//--------------------------------------
/// \brief 状态处理

double getTdAngle(const double vect1[3], const double vect2[3]) {
	double vectDot = vect1[0] * vect2[0] + vect1[1] * vect2[1] + vect1[2] * vect2[2];
	double vectMod1 = sqrt( POW2(vect1[0]) + POW2(vect1[1]) + POW2(vect1[2]) );
	double vectMod2 = sqrt( POW2(vect2[0]) + POW2(vect2[1]) + POW2(vect2[2]) );
	if (vectMod1 == 0 || vectMod2 == 0) return 0;
	double cosValue = vectDot / (vectMod1 * vectMod2);
	double rad = acos(cosValue);
	return rad;
}

inline void MyStrategy::PrintState(bool b) {
	if (flightRole == 1) PrintStatus(LEAD_STATE, GetNearestTgt(), b);
	else PrintStatus(WING_STATE, GetNearestTgt(), b);
}

void MyStrategy::PrintStatus(const char * filename, ACAI::ACRdrTarget::RdrTgtInfo nearestTgt, bool b) {

	static const double dis2Lons = 160000 / (mPKConfig.RightUpLon - mPKConfig.LeftDownLon);
	static const double dis2Lats = 80000 / (mPKConfig.RightUpLat - mPKConfig.LeftDownLat);

	double r;
	double s = nearestTgt.slantRange;
	if (s>50000)
		s = 50000;
	if (b == false) {r = 0;}
	else {r = OutputReward();}
	int nCoMslCnt = 0;
	for (int i = 0; i < mCOMSLInGuide.flightMemCnt; ++i) {
		nCoMslCnt += mCOMSLInGuide.memMSLInGuide[0].mslCnt;
	}
	FILE *fp = fopen(filename, "a");
	fprintf(fp, "[%d], %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %d, %d, %d, %d, %d, %d, %lf\n",
		mACFlightStatus.timeCounter,						// [时标]
		s,													// 目标距离
		mACFlightStatus.alt - nearestTgt.alt,				// 目标高度差
		nearestTgt.azGeo,									// 目标 地理系 方位角
		nearestTgt.sbsSpeed,								// 目标速度(m/s)
		mCOFlightStatus.flightMemCnt ?
		mCOFlightStatus.memFlightStatus[0].groundSpeed :	// 友机速度(m/s)
		0,
		mCOFlightStatus.flightMemCnt ?
		sqrt( POW2(mACFlightStatus.alt - mCOFlightStatus.memFlightStatus[0].alt) +
		POW2((mACFlightStatus.lon - mCOFlightStatus.memFlightStatus[0].lon) * dis2Lons) +
		POW2((mACFlightStatus.lat - mCOFlightStatus.memFlightStatus[0].lat) * dis2Lats) ) :
		MAX_DIS,											// 友机距离
		mCOFlightStatus.flightMemCnt ?
		mACFlightStatus.alt - mCOFlightStatus.memFlightStatus[0].alt :
		MAX_DIS,											// 友机高度差
		getTdAngle(mACFlightStatus.velNWU, mCOFlightStatus.memFlightStatus[0].velNWU),
															// 友机速度矢量夹角
		mACFlightStatus.groundSpeed,						// 本机速度
		getTdAngle(mACFlightStatus.velNWU, nearestTgt.velNWU),
															// 敌机速度矢量夹角
		mACMslWarning.mslCnt > 0,							// 是否被导弹锁定
		mACMSLInGuide.mslCnt > 0,							// 是否在制导
		mACFlightStatus.remainWpnNum,						// 剩余武器量
		GetTgtSum(),										// 扫描到的敌机数量
		mACMslWarning.mslCnt,								// 锁定自己的导弹数量
		mACMSLInGuide.mslCnt + nCoMslCnt,					// 场上我方导弹数量
		r
		);
	fclose(fp);
}

//--------------------------------------
/// \brief 回报处理
void MyStrategy::SetFlightState(unsigned int flightID)
{
	// 判断友方（或者自身）飞机被击落的情况，返回状态值2
	if ((flightID == mACFlightStatus.flightID) || (flightID == mCOFlightStatus.memFlightStatus[0].flightID)) {
		g_flight_state = 2;
	}
	// 判断敌方飞机被击落的情况，返回状态值1
	else {
		g_flight_state = 1;
	}
}

double MyStrategy::OutputReward()
{
	double reward = 0;

	// 敌方飞机被击落的情况（击落一架敌机）
	if(g_flight_state == 1)
	{
		// 重置状态变量
		g_flight_state = 0;
		reward += 125.0;
	}
	// 友方飞机或自身被击落的情况（友方飞机被击落）
	if(g_flight_state == 2)
	{
		// 重置状态变量
		g_flight_state = 0;
		reward -= 125.0;
	}

	if(g_win_state == 1)
	{
		// 重置状态变量
		g_win_state = 0;
		reward += 300.0;
	}

	if(g_win_state == 2)
	{
		// 重置状态变量
		g_win_state = 0;
		reward -= 300.0;
	}

	// 当导弹威胁数增加且没有飞机被击中时的reward（我方被敌方导弹锁定）
	// 当导弹威胁数减少并且没有飞机坠毁时的reward（我方逃脱敌方导弹）
	reward -= 20 * ((double)mACMslWarning.mslCnt - (double)g_cnt_state);
	g_cnt_state = mACMslWarning.mslCnt;
	reward -= 10 * (double)mACMslWarning.mslCnt;

	//// 新侦查到敌机的情况（我方获得敌方视野）
	//// 侦察到的敌机数量减少（我方丢失敌方视野）
	reward += 3.6 * ((double)mACRdrTarget.tgtCnt - (double)g_enmy_state);
	g_enmy_state = mACRdrTarget.tgtCnt;

	reward += 1.2 * (double)mACRdrTarget.tgtCnt;

	//// 飞机发射导弹
	reward -= 10.0 * ((double)mACFlightStatus.remainWpnNum - (double)g_launch_state);
	g_launch_state = mACFlightStatus.remainWpnNum;
	
	reward += 3*CalDisAdv();

	reward += 3*CalAltAdv();

	reward += 3*CalAngAdv();

	//reward += CalWinAdv();
	return reward;
}

int MyStrategy::GetTgtSum() {
	set<unsigned int> tgtSet;
	if ((mACRdrTarget.tgtCnt == 0) && (mCORdrTarget.memRdrTarget[0].tgtCnt == 0))
		return 0;
	for (int i = 0; i < mACRdrTarget.tgtCnt; i++)
		tgtSet.insert(mACRdrTarget.tgtInfos[i].tgtID);
	for (int i = 0; i < mCORdrTarget.memRdrTarget[0].tgtCnt; i++)
		tgtSet.insert(mCORdrTarget.memRdrTarget[0].tgtInfos[i].tgtID);
	return tgtSet.size();
}

ACAI::ACRdrTarget::RdrTgtInfo MyStrategy::GetNearestTgt() {
	if (GetTgtSum() == 1) {
		if (mACRdrTarget.tgtCnt == 1)
			return mACRdrTarget.tgtInfos[0];
		else {
			ACAI::ACRdrTarget::RdrTgtInfo tgtInfoTemp = mCORdrTarget.memRdrTarget[0].tgtInfos[0];
			TriSolveResult mTriSolveResult = SolveTriangle(mACFlightStatus, mCORdrTarget.memRdrTarget[0].tgtInfos[0]);
			tgtInfoTemp.azGeo = mTriSolveResult.angle;
			tgtInfoTemp.slantRange = mTriSolveResult.length;
			return tgtInfoTemp;
		}
	}
	if (GetTgtSum() == 2) {
		if (mACRdrTarget.tgtCnt == 2) {
			if (mACRdrTarget.tgtInfos[0].slantRange <= mACRdrTarget.tgtInfos[1].slantRange)
				return mACRdrTarget.tgtInfos[0];
			else
				return mACRdrTarget.tgtInfos[1];
		}
		if (mACRdrTarget.tgtCnt == 1) {
			ACAI::ACRdrTarget::RdrTgtInfo tgtInfoTemp0 = mCORdrTarget.memRdrTarget[0].tgtInfos[0];
			ACAI::ACRdrTarget::RdrTgtInfo tgtInfoTemp1 = mCORdrTarget.memRdrTarget[0].tgtInfos[1];
			TriSolveResult mTriSolveResult0 = SolveTriangle(mACFlightStatus, mCORdrTarget.memRdrTarget[0].tgtInfos[0]);
			TriSolveResult mTriSolveResult1 = SolveTriangle(mACFlightStatus, mCORdrTarget.memRdrTarget[0].tgtInfos[1]);
			tgtInfoTemp0.azGeo = mTriSolveResult0.angle;
			tgtInfoTemp0.slantRange = mTriSolveResult0.length;
			tgtInfoTemp1.azGeo = mTriSolveResult1.angle;
			tgtInfoTemp1.slantRange = mTriSolveResult1.length;
			if (mACRdrTarget.tgtInfos[0].tgtID == tgtInfoTemp0.tgtID)
				tgtInfoTemp0 = mACRdrTarget.tgtInfos[0];
			else
				tgtInfoTemp1 = mACRdrTarget.tgtInfos[0];
			if ((tgtInfoTemp0.slantRange + mCORdrTarget.memRdrTarget[0].tgtInfos[0].slantRange) < (tgtInfoTemp1.slantRange + mCORdrTarget.memRdrTarget[0].tgtInfos[1].slantRange))
				return tgtInfoTemp0;
			else
				return tgtInfoTemp1;
		}
		else {
			ACAI::ACRdrTarget::RdrTgtInfo tgtInfoTemp0 = mCORdrTarget.memRdrTarget[0].tgtInfos[0];
			ACAI::ACRdrTarget::RdrTgtInfo tgtInfoTemp1 = mCORdrTarget.memRdrTarget[0].tgtInfos[1];
			TriSolveResult mTriSolveResult0 = SolveTriangle(mACFlightStatus, mCORdrTarget.memRdrTarget[0].tgtInfos[0]);
			TriSolveResult mTriSolveResult1 = SolveTriangle(mACFlightStatus, mCORdrTarget.memRdrTarget[0].tgtInfos[1]);
			tgtInfoTemp0.azGeo = mTriSolveResult0.angle;
			tgtInfoTemp0.slantRange = mTriSolveResult0.length;
			tgtInfoTemp1.azGeo = mTriSolveResult1.angle;
			tgtInfoTemp1.slantRange = mTriSolveResult1.length;
			if ((tgtInfoTemp0.slantRange + mCORdrTarget.memRdrTarget[0].tgtInfos[0].slantRange) < (tgtInfoTemp1.slantRange + mCORdrTarget.memRdrTarget[0].tgtInfos[1].slantRange))
				return tgtInfoTemp0;
			else
				return tgtInfoTemp1;
		}
	}
	ACAI::ACRdrTarget::RdrTgtInfo tgtInfoTemp;
	memset(&tgtInfoTemp, 0, sizeof(tgtInfoTemp));
	return tgtInfoTemp;
}

TriSolveResult MyStrategy::SolveTriangle(ACAI::ACFlightStatus mACFlightStatus, ACAI::ACRdrTarget::RdrTgtInfo fRdrTgtInfo) {
	TriSolveResult result;
	memset(&result, 0, sizeof(result));
	result.angle = 0;
	result.length = 0;
	double mLon = mACFlightStatus.lon;
	double mLat = mACFlightStatus.lat;
	double mAlt = mACFlightStatus.alt;
	double tLon = fRdrTgtInfo.lon;
	double tLat = fRdrTgtInfo.lat;
	double tAlt = fRdrTgtInfo.alt;

	double mtDis = sqrt( POW2(mAlt - tAlt) + POW2((mLon - tLon) * dis2Lons) + POW2((mLat - tLat) * dis2Lats) );
	double cosA = fabs(((mLat - tLat) * dis2Lats) / (sqrt(POW2((mLon - tLon) * dis2Lons) + POW2((mLat - tLat) * dis2Lats))));
	double A = acos((cosA > 0.99) ? 0.99 : (cosA < -0.99) ? -0.99 : cosA);

	if ((tLon > mLon) && (tLat < mLat))
		A += (double)(PI * 0.5);
	else if ((tLon < mLon) && (tLat < mLat))
		A += (double)PI;
	else if ((tLon < mLon) && (tLat > mLat))
		A += (double)(PI * 1.5);

	result.length = mtDis;
	result.angle = A;

	return result;
};

double MyStrategy::CalDisAdv() {
	double dis = SolveTriangle(mACFlightStatus, GetNearestTgt()).length;
	double maxDis = 2000;
	if (mACFCCStatus.tgtCnt != 0)
		maxDis = mACFCCStatus.envInfos[0].FoeRtr;
	if (dis <= maxDis)
		return 1.5;
	else
		return 3 * exp(-POW2((dis - maxDis)/maxDis)) - 1.5;
}

double MyStrategy::CalAltAdv() {
	double H_m = 1500;
	double ang = fabs(getTdAngle(mACFlightStatus.velNWU,GetNearestTgt().velNWU));
	if (ang < PI * 0.5) {
		if (mACFlightStatus.alt - GetNearestTgt().alt > 0)
			return 3 * exp(-POW2((mACFlightStatus.alt - GetNearestTgt().alt - H_m) / H_m));
		else
			return -3 * exp(-POW2((GetNearestTgt().alt - mACFlightStatus.alt - H_m) / H_m));
	}
	else if (ang > PI * 0.5 && ang < PI) {
		if (mACFlightStatus.alt - GetNearestTgt().alt < 0)
			return 3 * exp(-POW2((mACFlightStatus.alt - GetNearestTgt().alt - H_m) / H_m));
		else
			return -3 * exp(-POW2((GetNearestTgt().alt - mACFlightStatus.alt - H_m) / H_m));
	}
	else
		return 0;
}

double MyStrategy::CalAngAdv() {
	double ang = fabs(getTdAngle(mACFlightStatus.velNWU,GetNearestTgt().velNWU));
	if (ang < PI * 0.5)
		return 3 * (PI * 0.5 - ang);
	else if (ang > PI * 0.5 && ang < PI)
		return 3 * (PI * 0.5 - ang);
	else
		return 0;
}

double MyStrategy::CalWinAdv() {
	double mLon = mACFlightStatus.lon;
	double mLat = mACFlightStatus.lat;
	double tLon;
	double tLat;

	if( ACAI::V_FLIGHT_TEAM_RED == mACFlightStatus.flightTeam )
	{
		tLon = mPKConfig.RedMissionLon;    
		tLat = mPKConfig.RedMissionLat;    
	}
	else
	{
		tLon = mPKConfig.BlueMissionLon;   
		tLat = mPKConfig.BlueMissionLat;    
	}

	double dis = sqrt( POW2((mLon - tLon) * dis2Lons) + POW2((mLat - tLat) * dis2Lats) );
	double rate = 1 - dis/160000;
	if (mLon > 0.5 * (mPKConfig.RightUpLon - mPKConfig.LeftDownLon))
		return POW2(exp(rate));
	else
		return -POW2(exp(rate));
}

bool MyStrategy::judge() {
	double mLon = mACFlightStatus.lon;
	double mLat = mACFlightStatus.lat;
	double fLon = mCOFlightStatus.memFlightStatus[0].lon;
	double fLat = mCOFlightStatus.memFlightStatus[0].lat;
	double tLon;
	double tLat;

	if( ACAI::V_FLIGHT_TEAM_RED == mACFlightStatus.flightTeam )
	{
		tLon = mPKConfig.RedMissionLon;    
		tLat = mPKConfig.RedMissionLat;    
	}
	else
	{
		tLon = mPKConfig.BlueMissionLon;   
		tLat = mPKConfig.BlueMissionLat;    
	}

	double rate = sqrt( POW2((mLon - tLon) * dis2Lons) + POW2((mLat - tLat) * dis2Lats) );
	double rate_2 = sqrt( POW2((fLon - tLon) * dis2Lons) + POW2((fLat - tLat) * dis2Lats) );
	if (rate < 50 || rate_2 < 50)
		return true;
	else
		return false;
}