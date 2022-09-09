#include "my_strategy.h"
#include "string.h"
#include <iostream>
using namespace std;

#define MAX_DIS 160000
#define POW2(x) (x) * (x)

bool g_fight_init;
int tgt_num;

/// \brief 比赛开始时执行
void MyStrategy::onPKStart(const ACAI::PKConfig &pkConfig)
{
	memcpy(&mPKConfig, &pkConfig, sizeof(mPKConfig));
	g_fight_init = false;
	tgt_num = GetTgtSum();

	initData(2);
}

/// \brief 比赛结束时执行 
void MyStrategy::onPKEnd()
{
	g_fight_init = false;
}

void MyStrategy::onPKOut(unsigned int flightID)
{
	// 判断友方（或者自身）飞机被击落的情况，返回状态值2
	if ((flightID == mACFlightStatus.flightID) || (flightID == mCOFlightStatus.memFlightStatus[0].flightID)) {
		m_fallen_num += 1;
	}
	// 判断敌方飞机被击落的情况，返回状态值1
	else {
		t_fallen_num += 1;
	}
}

void MyStrategy::backGround()
{

}

void MyStrategy::timeSlice20()
{

}

/// \brief 40ms线程
void MyStrategy::timeSlice40()
{
	tgt_num = GetTgtSum();
	if (g_fight_init == false) {
		write_maneuver(13);
		if (tgt_num > 0) {
			g_fight_init = true;
		}
	}

	if (action_finished && g_fight_init) {
		write_maneuver( PrintStatus(GetNearestTgt()) );
	}
	
	maneuver_i(mCmd);
	DoTacWpnShoot();

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

double cal_log(double data) {
	if (data < 1)
		return 0;
	else
		return log(data);
}

int MyStrategy::PrintStatus(ACAI::ACRdrTarget::RdrTgtInfo nearestTgt) {
	double t_slant = nearestTgt.slantRange;
	if (t_slant > 160000)
		t_slant = 160000;

	double tLon, tLat;
	double mLon = mACFlightStatus.lon;
	double mLat = mACFlightStatus.lat;
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
	double tDis = sqrt( POW2((mLon - tLon) * dis2Lons) + POW2((mLat - tLat) * dis2Lats) );

	double t_ang = nearestTgt.azGeo;

	int nCoMslCnt = 0;
	for (int i = 0; i < mCOMSLInGuide.flightMemCnt; ++i) {
		nCoMslCnt += mCOMSLInGuide.memMSLInGuide[0].mslCnt;
	}
	char cmdLine[1024];
	sprintf(cmdLine, "Assist\\Predict.exe %d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %d %d %d %d %d %d %lf %d",
		flightRole,											// 角色
		mACFlightStatus.timeCounter,						// 时标
		cal_log(t_slant),									// 目标距离对数
		(mACFlightStatus.alt - nearestTgt.alt)/100,			// 目标高度差
		t_ang,												// 目标 地理系 方位角
		nearestTgt.sbsSpeed / 100,							// 目标速度(100m/s)
		mCOFlightStatus.flightMemCnt ?
		mCOFlightStatus.memFlightStatus[0].groundSpeed / 100 :	// 友机速度(100m/s)
		0,
		cal_log(mCOFlightStatus.flightMemCnt ?
		sqrt( POW2(mACFlightStatus.alt - mCOFlightStatus.memFlightStatus[0].alt) +
		POW2((mACFlightStatus.lon - mCOFlightStatus.memFlightStatus[0].lon) * dis2Lons) +
		POW2((mACFlightStatus.lat - mCOFlightStatus.memFlightStatus[0].lat) * dis2Lats) ) :
		MAX_DIS),											// 友机距离对数
		(mCOFlightStatus.flightMemCnt ?
		mACFlightStatus.alt - mCOFlightStatus.memFlightStatus[0].alt :
		12000)/100,											// 友机高度差
		getTdAngle(mACFlightStatus.velNWU, mCOFlightStatus.memFlightStatus[0].velNWU),
															// 友机速度矢量夹角
		mACFlightStatus.groundSpeed / 100,					// 本机速度(100m/s)
		getTdAngle(mACFlightStatus.velNWU, nearestTgt.velNWU),
															// 敌机速度矢量夹角
		mACMslWarning.mslCnt > 0,							// 是否被导弹锁定
		mACMSLInGuide.mslCnt > 0,							// 是否在制导
		mACFlightStatus.remainWpnNum,						// 剩余武器量
		GetTgtSum(),										// 扫描到的敌机数量
		mACMslWarning.mslCnt,								// 锁定自己的导弹数量
		mACMSLInGuide.mslCnt + nCoMslCnt,					// 场上我方导弹数量
		log(tDis),											// 與終點距離对数
		train_config.tgt_num - t_fallen_num					// 剩餘敵機數量
		);
	int iRet = system(cmdLine);
	return iRet;
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
			if (mACEwsTarget.tgtCnt != 0) {
				tgtInfoTemp.azBody = mACEwsTarget.tgtInfos[0].azBody;
				tgtInfoTemp.azGeo = mACEwsTarget.tgtInfos[0].azGeo;
			}
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
			if (mACEwsTarget.tgtCnt != 0) {
				for (int i=0; i<mACEwsTarget.tgtCnt; i++) {
					if (mACEwsTarget.tgtInfos[i].tgtID == tgtInfoTemp0.tgtID) {
						tgtInfoTemp0.azBody = mACEwsTarget.tgtInfos[i].azBody;
						tgtInfoTemp0.azGeo = mACEwsTarget.tgtInfos[i].azGeo;
					}
					if (mACEwsTarget.tgtInfos[i].tgtID == tgtInfoTemp1.tgtID) {
						tgtInfoTemp1.azBody = mACEwsTarget.tgtInfos[i].azBody;
						tgtInfoTemp1.azGeo = mACEwsTarget.tgtInfos[i].azGeo;
					}
				}
			}
			if (mACRdrTarget.tgtInfos[0].tgtID == tgtInfoTemp0.tgtID)
				tgtInfoTemp0 = mACRdrTarget.tgtInfos[0];
			else
				tgtInfoTemp1 = mACRdrTarget.tgtInfos[0];
			if (mCORdrTarget.memRdrTarget[0].tgtInfos[0].slantRange < mCORdrTarget.memRdrTarget[0].tgtInfos[1].slantRange)
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
			if (mACEwsTarget.tgtCnt != 0) {
				for (int i=0; i<mACEwsTarget.tgtCnt; i++) {
					if (mACEwsTarget.tgtInfos[i].tgtID == tgtInfoTemp0.tgtID) {
						tgtInfoTemp0.azBody = mACEwsTarget.tgtInfos[i].azBody;
						tgtInfoTemp0.azGeo = mACEwsTarget.tgtInfos[i].azGeo;
					}
					if (mACEwsTarget.tgtInfos[i].tgtID == tgtInfoTemp1.tgtID) {
						tgtInfoTemp1.azBody = mACEwsTarget.tgtInfos[i].azBody;
						tgtInfoTemp1.azGeo = mACEwsTarget.tgtInfos[i].azGeo;
					}
				}
			}
			if (mCORdrTarget.memRdrTarget[0].tgtInfos[0].slantRange < mCORdrTarget.memRdrTarget[0].tgtInfos[1].slantRange)
				return tgtInfoTemp0;
			else
				return tgtInfoTemp1;
		}
	}
	ACAI::ACRdrTarget::RdrTgtInfo tgtInfoTemp;
	memset(&tgtInfoTemp, 0, sizeof(tgtInfoTemp));
	if (mACEwsTarget.tgtCnt != 0) {
		tgtInfoTemp.azBody = mACEwsTarget.tgtInfos[0].azBody;
		tgtInfoTemp.azGeo = mACEwsTarget.tgtInfos[0].azGeo;
	}
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