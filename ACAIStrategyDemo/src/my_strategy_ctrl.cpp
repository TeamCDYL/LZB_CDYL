#include "my_strategy.h"
#include "string.h"
#include <iostream>
using namespace std;

#define LEAD_STATE "..\\Assist\\Data\\Lead\\state1.csv"
#define WING_STATE "..\\Assist\\Data\\Wing\\state2.csv"
#define LEAD_ACTION "..\\Assist\\Data\\Lead\\action1.csv"
#define WING_ACTION "..\\Assist\\Data\\Wing\\action2.csv"
#define START "..\\Assist\\Data\\start"
#define END "..\\Assist\\Data\\end"
#define FIGHT "..\\Assist\\Data\\fight"
#define OUTFT "..\\Assist\\Data\\outft"
#define MAX_DIS 160000
#define POW2(x) (x) * (x)

const char * StateTitle = "[time counter], distance to enemy, alt distance to enemy, "
						"direction to enemy, enemy speed, friend speed, "
						"distance to friend, alt distance to friend, speed direct to friend, "
						"self speed, speed direct to enemy, is locked by msl, "
						"is in guide, remaining msl, enemy in rdr, "
						"n msl locked, our n msl, dis to end, remain tgt n, reward\n";
const char * ActionTitle = "action first index, action second index\n";

unsigned int g_flight_state;	///< �ɻ����״̬
unsigned int g_cnt_state;		///< ������в״̬
unsigned int g_enmy_state;		///< �л�����״̬
unsigned int g_wpn_state;
bool g_fight_init;
int tgt_num;

/// \brief ������ʼʱִ��
void MyStrategy::onPKStart(const ACAI::PKConfig &pkConfig)
{
	memcpy(&mPKConfig, &pkConfig, sizeof(mPKConfig));
	g_flight_state = 0;	///< �ɻ����״̬
	g_cnt_state = 0;	///< ������в״̬
	g_enmy_state = 0;	///< �л�����״̬
	g_wpn_state = mACFlightStatus.remainWpnNum;
	g_fight_init = false;
	tgt_num = GetTgtSum();

	initData(2);

	if (flightRole == 1) {
		remove(START);
		remove(END);
		remove(FIGHT);
		remove(OUTFT);
		FILE *tmp_1 = fopen(START, "w");
		fclose(tmp_1);
		FILE *tmp_2 = fopen(OUTFT, "w");
		fclose(tmp_2);

		remove(LEAD_STATE);
		remove(LEAD_ACTION);
		FILE *tmp = fopen(LEAD_STATE, "w"); // ����ļ�����
		fprintf(tmp, StateTitle);			// �ļ���ͷ
		fclose(tmp);
		tmp = fopen(LEAD_ACTION, "w");
		fprintf(tmp, ActionTitle);
		fclose(tmp);
	} else {
		remove(WING_STATE);
		remove(WING_ACTION);
		FILE *tmp = fopen(WING_STATE, "w"); // ����ļ�����
		fprintf(tmp, StateTitle);			// �ļ���ͷ
		fclose(tmp);
		tmp = fopen(WING_ACTION, "w");
		fprintf(tmp, ActionTitle);
		fclose(tmp);
	}
}

/// \brief ��������ʱִ�� 
void MyStrategy::onPKEnd()
{
	double r = OutputReward();
	if(judge(5001, true) || t_fallen_num >= train_config.tgt_num)
		r += train_config.win;
	else
		r -= train_config.win;

	PrintState(r);

	g_fight_init = false;
	initData(1);

	if (flightRole == 1) {
		remove(START);
		remove(END);
		remove(FIGHT);
		remove(OUTFT);
		FILE *tmp_1 = fopen(END, "w");
		fclose(tmp_1);
	}
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

/// \brief 40ms�߳�
void MyStrategy::timeSlice40()
{
	tgt_num = GetTgtSum();
	if (g_fight_init == false) {
		write_maneuver(1, 3);
		if (tgt_num > 0) {
			PrintState(OutputReward());
			g_fight_init = true;
			if (flightRole == 1) {
				// ��һ�ν�����ս״̬�ĳ�ʼ��
				FILE *tmp = fopen(FIGHT, "w"); // ��־������ս״̬
				fclose(tmp);
				remove(OUTFT);
			}
		}
	}

	if (action_finished && g_fight_init)
	{
#ifdef DEEP_LEARNING	// ���ѧϰʱ��ȡ����
		static bool isReadAction = false;
		if (isReadAction) PrintState(OutputReward());
		isReadAction = readAction();
#endif

#ifndef DEEP_LEARNING // ר��ģʽʱִ�ж���
		//����ս��
		static int mslWarningStartTime = 0;
		if (mACMslWarning.mslCnt > 0) {	
			if (mACFlightStatus.timeCounter - mslWarningStartTime > 3000) {		// �������濪ʼ3�����
				readActionByPro(0, 11);
				//DoTacHeadEvade();
				if (GetTgtSum() > 0){
					PrintState();
					PrintReward();
				}
				strcpy(outputData.EventDes, "��ͷ");
				logEvent(outputData);
			}
			else { // ��û�е�������ʱˢ�µ��������ʱ��
				mslWarningStartTime = mACFlightStatus.timeCounter;
			}
		}
		// �޷�������������ʱȫ��ͻ��
		if (mACRdrTarget.tgtCnt == 0 &&	mACMslWarning.mslCnt ==0) {
			readActionByPro(0, 1);
			//DoTacPointAtk();
				if (GetTgtSum() > 0){
					PrintState();
					PrintReward();
				}
			strcpy(outputData.EventDes, "��з�����ͻΧ");
			logEvent(outputData);
		}
		if (mACRdrTarget.tgtCnt > 0 && mACMslWarning.mslCnt == 0 && mACMSLInGuide.mslCnt > 0) {
			// �ж�׷������ͻ��
			// ���ڵ���ʱ
			if (mACFCCStatus.envInfos[0].FPoleValid || mACFCCStatus.envInfos[0].APoleValid) {
				readActionByPro(1, 1);
				//DoTacWpnShoot();
				if (GetTgtSum() > 0){
					PrintState();
					PrintReward();
				}
				strcpy(outputData.EventDes, "��������");
				logEvent(outputData);
			}
			if (mACFCCStatus.envInfos[1].FPoleValid || mACFCCStatus.envInfos[1].APoleValid) {
				readActionByPro(1, 3);
				//DoTacWpnShoot(1);
				if (GetTgtSum() > 0){
					PrintState();
					PrintReward();
				}
				strcpy(outputData.EventDes,"��������");
				logEvent(outputData);
			} else {
				readActionByPro(0, 2);
				//DoTacToTar();
				if (GetTgtSum() > 0){
					PrintState();
					PrintReward();
				}
				strcpy(outputData.EventDes, "��һ�ܵз��ɻ�����");
				logEvent(outputData);
			}
		}
		if (mACRdrTarget.tgtCnt > 0 && mACMslWarning.mslCnt == 0 && mACMSLInGuide.mslCnt == 0) {
			readActionByPro(0, 4);
			//DoTacAltClimbP60();
			PrintState();
			PrintReward();
			strcpy(outputData.EventDes, "��60������");
			logEvent(outputData);
			if (mACFlightStatus.alt==4000) {
				readActionByPro(0, 1);
				//DoTacPointAtk();
				if (GetTgtSum() > 0){
					PrintState();
					PrintReward();
				}
				strcpy(outputData.EventDes, "��з�����ͻϮ");
				logEvent(outputData);
			}
		}
#endif
	} 
	
	maneuver_i(mCmd.fin, mCmd.sin);
	DoTacWpnShoot();

	//����
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
/// \brief ��������
struct Action {
	int fin, sin; // һ��������0�����У�1���������������������嶯��
};

bool first_read = true;
void deal(const char* filename, LPVOID lParam) { // ����ԭ�򲻷���ӵ�MyStrategy����
	if (first_read) {
		first_read = false;
		return;
	}
	FILE* fp = fopen(filename, "r");
	int fin, sin; // һ����������������
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
		} else { // �����ڼ��ļ�δ�����ı�
			return false;
		}
	} else {
		if (watch(WING_ACTION, deal, &act)) {
			write_maneuver(act.fin, act.sin);
			return true;
		} else { // �����ڼ��ļ�δ�����ı�
			return false;
		}
	}
	return false;
}

void MyStrategy::readActionByPro(int fin, int sin) { // һ��������
	maneuver_i(fin, sin);
#ifndef DEEP_LEARNING // ר��ģʽʱ�������
	// ��ս�����
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
/// \brief ״̬����

double getTdAngle(const double vect1[3], const double vect2[3]) {
	double vectDot = vect1[0] * vect2[0] + vect1[1] * vect2[1] + vect1[2] * vect2[2];
	double vectMod1 = sqrt( POW2(vect1[0]) + POW2(vect1[1]) + POW2(vect1[2]) );
	double vectMod2 = sqrt( POW2(vect2[0]) + POW2(vect2[1]) + POW2(vect2[2]) );
	if (vectMod1 == 0 || vectMod2 == 0) return 0;
	double cosValue = vectDot / (vectMod1 * vectMod2);
	double rad = acos(cosValue);
	return rad;
}

inline void MyStrategy::PrintState(double reward) {
	if (flightRole == 1) PrintStatus(LEAD_STATE, GetNearestTgt(), reward);
	else PrintStatus(WING_STATE, GetNearestTgt(), reward);
}

double cal_log(double data) {
	if (data < 1)
		return 0;
	else
		return log(data);
}

void MyStrategy::PrintStatus(const char * filename, ACAI::ACRdrTarget::RdrTgtInfo nearestTgt, double reward) {
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
	FILE *fp = fopen(filename, "a");
	fprintf(fp, "[%d], %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %d, %d, %d, %d, %d, %d, %lf, %d, %lf\n",
		mACFlightStatus.timeCounter,						// [ʱ��]
		cal_log(t_slant),										// Ŀ��������
		(mACFlightStatus.alt - nearestTgt.alt)/100,			// Ŀ��߶Ȳ�
		t_ang,												// Ŀ�� ����ϵ ��λ��
		nearestTgt.sbsSpeed / 100,							// Ŀ���ٶ�(100m/s)
		mCOFlightStatus.flightMemCnt ?
		mCOFlightStatus.memFlightStatus[0].groundSpeed / 100 :	// �ѻ��ٶ�(100m/s)
		0,
		cal_log(mCOFlightStatus.flightMemCnt ?
		sqrt( POW2(mACFlightStatus.alt - mCOFlightStatus.memFlightStatus[0].alt) +
		POW2((mACFlightStatus.lon - mCOFlightStatus.memFlightStatus[0].lon) * dis2Lons) +
		POW2((mACFlightStatus.lat - mCOFlightStatus.memFlightStatus[0].lat) * dis2Lats) ) :
		MAX_DIS),											// �ѻ��������
		(mCOFlightStatus.flightMemCnt ?
		mACFlightStatus.alt - mCOFlightStatus.memFlightStatus[0].alt :
		12000)/100,											// �ѻ��߶Ȳ�
		getTdAngle(mACFlightStatus.velNWU, mCOFlightStatus.memFlightStatus[0].velNWU),
															// �ѻ��ٶ�ʸ���н�
		mACFlightStatus.groundSpeed / 100,					// �����ٶ�(100m/s)
		getTdAngle(mACFlightStatus.velNWU, nearestTgt.velNWU),
															// �л��ٶ�ʸ���н�
		mACMslWarning.mslCnt > 0,							// �Ƿ񱻵�������
		mACMSLInGuide.mslCnt > 0,							// �Ƿ����Ƶ�
		mACFlightStatus.remainWpnNum,						// ʣ��������
		GetTgtSum(),										// ɨ�赽�ĵл�����
		mACMslWarning.mslCnt,								// �����Լ��ĵ�������
		mACMSLInGuide.mslCnt + nCoMslCnt,					// �����ҷ���������
		log(tDis),											// �c�K�c���x����
		train_config.tgt_num - t_fallen_num,				// ʣ�N���C����
		reward
		);
	fclose(fp);
}

//--------------------------------------
/// \brief �ر�����
void MyStrategy::SetFlightState(unsigned int flightID)
{
	// �ж��ѷ������������ɻ�����������������״ֵ̬2
	if ((flightID == mACFlightStatus.flightID) || (flightID == mCOFlightStatus.memFlightStatus[0].flightID)) {
		g_flight_state = 2;
		m_fallen_num += 1;
	}
	// �жϵз��ɻ�����������������״ֵ̬1
	else {
		g_flight_state = 1;
		t_fallen_num += 1;
	}
}

double MyStrategy::OutputReward()
{
	double reward = 0;

	if (mACFlightStatus.remainWpnNum < g_wpn_state && g_flight_state != 2) {
		reward -= train_config.lost_wpn;
		g_wpn_state = mACFlightStatus.remainWpnNum;
	}

	// �з��ɻ�����������������һ�ܵл���
	if(g_flight_state == 1)
	{
		// ����״̬����
		g_flight_state = 0;
		reward += train_config.shoot_down;
	}
	// �ѷ��ɻ������������������ѷ��ɻ������䣩
	if(g_flight_state == 2)
	{
		// ����״̬����
		g_flight_state = 0;
		reward -= train_config.shoot_down;
	}

	// ��������в��������û�зɻ�������ʱ��reward���ҷ����з�����������
	// ��������в�����ٲ���û�зɻ�׹��ʱ��reward���ҷ����ѵз�������
	reward -= train_config.in_warning * ((double)mACMslWarning.mslCnt - (double)g_cnt_state);
	g_cnt_state = mACMslWarning.mslCnt;
	reward -= train_config.out_warning * (double)mACMslWarning.mslCnt;

	//// ����鵽�л���������ҷ���õз���Ұ��
	//// ��쵽�ĵл��������٣��ҷ���ʧ�з���Ұ��
	reward += train_config.get_target * ((double)mACRdrTarget.tgtCnt - (double)g_enmy_state);
	g_enmy_state = mACRdrTarget.tgtCnt;

	reward += 0.3 * train_config.get_target * (double)mACRdrTarget.tgtCnt;

	reward += train_config.dis_adv * CalDisAdv();
	reward += train_config.alt_adv * CalAltAdv();
	reward += train_config.ang_adv * CalAngAdv();
	reward += train_config.win_dav * CalWinAdv();

	if (judge(10000, false)) {
		reward += 40;
	}
	else if (judge(20000, false)) {
		reward += 20;
	}
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

double MyStrategy::CalDisAdv() {
	double dis = SolveTriangle(mACFlightStatus, GetNearestTgt()).length;
	double maxDis = 5000;
	if (mACFCCStatus.tgtCnt != 0)
		maxDis = mACFCCStatus.envInfos[0].FoeRtr;
	if (dis <= maxDis)
		return 1.5;
	else
		return exp(-POW2((dis - maxDis)/maxDis)) - 1.5;
}

double MyStrategy::CalAltAdv() {
	double H_m = 1500;
	double ang = fabs(getTdAngle(mACFlightStatus.velNWU,GetNearestTgt().velNWU));
	if (ang < PI * 0.5) {
		if (mACFlightStatus.alt - GetNearestTgt().alt > 0)
			return -exp(-POW2((mACFlightStatus.alt - GetNearestTgt().alt - H_m) / H_m));
		else
			return exp(-POW2((GetNearestTgt().alt - mACFlightStatus.alt - H_m) / H_m));
	}
	else if (ang > PI * 0.5 && ang < PI) {
		if (mACFlightStatus.alt - GetNearestTgt().alt < 0)
			return -exp(-POW2((mACFlightStatus.alt - GetNearestTgt().alt - H_m) / H_m));
		else
			return exp(-POW2((GetNearestTgt().alt - mACFlightStatus.alt - H_m) / H_m));
	}
	else
		return 0;
}

double MyStrategy::CalAngAdv() {
	double ang = fabs(getTdAngle(mACFlightStatus.velNWU,GetNearestTgt().velNWU));
	if (ang < PI * 0.5)
		return (PI * 0.5 - ang);
	else if (ang > PI * 0.5 && ang < PI)
		return (PI * 0.5 - ang);
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

bool MyStrategy::judge(double dis, bool option) {
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
	if (option == true) {
		if (rate < dis || rate_2 < dis)
			return true;
		else
			return false;
	}
	else {
		if (rate < dis)
			return true;
		else
			return false;
	}
}