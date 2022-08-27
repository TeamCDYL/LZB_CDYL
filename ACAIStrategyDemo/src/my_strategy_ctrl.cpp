#include "my_strategy.h"
#include "string.h"
#include <iostream>
using namespace std;

#define LEAD_STATE "state1.csv"
#define WING_STATE "state2.csv"
#define LEAD_ACTION "action1.csv"
#define WING_ACTION "action2.csv"
#define MAX_DIS 50000

const char * StateTitle = "[time counter], distance to enemy, alt distance to enemy, "
						"direction to enemy, enemy speed, friend speed, "
						"distance to friend, alt distance to friend, speed direct to friend, "
						"self speed, speed direct to enemy, is locked by msl, "
						"is in guide, remaining msl, enemy in rdr, "
						"n msl locked, our n msl, reward\n";
const char * ActionTitle = "action first index, action second index";

int timecounter=0;
int testcounter=0;

int g_flight_state;	///< �ɻ����״̬
int g_cnt_state;	///< ������в״̬
int g_enmy_state;	///< �л�����״̬
int g_launch_state;	///< �ҷ����䵼��״̬
int g_guide_state;	///< �ҷ��Ƶ�����״̬
bool g_fight_init;

/// \brief ������ʼʱִ��
void MyStrategy::onPKStart(const ACAI::PKConfig &pkConfig)
{
	initData();
    memcpy(&mPKConfig, &pkConfig, sizeof(mPKConfig));

	remove("start");
	remove("end");
	remove("fight");
	remove("outfight");
	FILE *tmp_1 = fopen("start", "w"); // ��־��ս��ʼ
	fclose(tmp_1);
	FILE *tmp_2 = fopen("outfight", "w");
	fclose(tmp_2);

	g_fight_init = false;

	remove(LEAD_STATE);
	remove(WING_STATE);
	remove(LEAD_ACTION);
	remove(WING_ACTION);
	mACFlightStatus.flightRole = ACAI::V_FLIGHT_ROLE_LEAD;
	if (mACFlightStatus.flightRole == ACAI::V_FLIGHT_ROLE_LEAD) {
		FILE *tmp = fopen(LEAD_STATE, "w"); // ����ļ�����
		fprintf(tmp, StateTitle);			// �ļ���ͷ
		fclose(tmp);
		tmp = fopen(LEAD_ACTION, "w");
		fprintf(tmp, ActionTitle);
		fclose(tmp);
	} else {
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
	initData();
	remove("start");
	remove("end");
	remove("fight");
	remove("outfight");
	FILE *tmp = fopen("end", "w"); // ��־��ս����
	fclose(tmp);
	g_fight_init = false;
}

void MyStrategy::onPKOut(unsigned int flightID)
{
	SetFlightState(flightID);
}

void MyStrategy::backGround()
{
    // TODO : �˴�����û�����ִ�к�̨����
}

void MyStrategy::timeSlice20()
{

}

bool inRange(double angle, double range) {
	return (angle <= range && angle >= -range) ||
		(PI - angle <= range && PI - angle >= -range);
}

/// \brief 40ms�߳�
void MyStrategy::timeSlice40()
{
	timecounter++;
	testcounter++;
	ACAI::EventLog outputData;
	memset(&outputData, 0, sizeof(outputData));

	//////////////////////////////////////////////////
	if (GetTgtSum() > 0) {
		FILE *tmp = fopen("fight", "w"); // ��־������ս״̬
		fclose(tmp);
		remove("outfight");
		if (g_fight_init == false) {
			g_fight_init = true;
			PrintState();
		}
	}
	else {
		FILE *tmp = fopen("outfight", "w"); // ��־������ս״̬
		fclose(tmp);
		remove("fight");
	}
	///////////////////////////////////////////////////////

	//����

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
	timecounter %= 250;
	if (timecounter == 0)
	{
		if (!(GetTgtSum() > 0))
			DoTacPointAtk(false);

#ifdef DEEP_LEARNING	// ���ѧϰʱ��ȡ����
		static bool isReadAction = false;
		if (isReadAction) PrintState();
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
	while (fgetc(fp) != 10);
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
	if (mACFlightStatus.flightRole == ACAI::V_FLIGHT_ROLE_LEAD) {
		if (watch(LEAD_ACTION, deal, &act)) {
			maneuver_i(act.fin, act.sin);
			return true;
		} else { // �����ڼ��ļ�δ�����ı�
			//DoTacPointAtk(false);
			return false;
		}
	} else {
		if (watch(WING_ACTION, deal, &act)) {
			maneuver_i(act.fin, act.sin);
			return true;
		} else { // �����ڼ��ļ�δ�����ı�
			//DoTacPointAtk(false);
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
#define POW2(x) (x) * (x)

double getTdAngle(const double vect1[3], const double vect2[3]) {
	double vectDot = vect1[0] * vect2[0] + vect1[1] * vect2[1] + vect1[2] * vect2[2];
	double vectMod1 = sqrt( POW2(vect1[0]) + POW2(vect1[1]) + POW2(vect1[2]) );
	double vectMod2 = sqrt( POW2(vect2[0]) + POW2(vect2[1]) + POW2(vect2[2]) );
	if (vectMod1 == 0 || vectMod2 == 0) return 0;
	double cosValue = vectDot / (vectMod1 * vectMod2);
	double rad = acos(cosValue);
	return rad;
}

inline void MyStrategy::PrintState() {
	if (mACFlightStatus.flightRole == ACAI::V_FLIGHT_ROLE_LEAD) PrintStatus(LEAD_STATE, GetNearestTgt());
	else PrintStatus(WING_STATE, GetNearestTgt());
}

void MyStrategy::PrintStatus(const char * filename, ACAI::ACRdrTarget::RdrTgtInfo nearestTgt) {
	static const double dis2Lons = 160000 / (mPKConfig.RightUpLon - mPKConfig.LeftDownLon);
	static const double dis2Lats = 80000 / (mPKConfig.RightUpLat - mPKConfig.LeftDownLat);
	int nCoMslCnt = 0;
	for (int i = 0; i < mCOMSLInGuide.flightMemCnt; ++i) {
		nCoMslCnt += mCOMSLInGuide.memMSLInGuide[0].mslCnt;
	}
	FILE *fp = fopen(filename, "a");
	fprintf(fp, "[%d], %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %d, %d, %d, %d, %d, %d, %d\n",
		mACFlightStatus.timeCounter,						// [ʱ��]
		nearestTgt.slantRange,								// Ŀ�����
		mACFlightStatus.alt - nearestTgt.alt,				// Ŀ��߶Ȳ�
		nearestTgt.azGeo,									// Ŀ�� ����ϵ ��λ��
		nearestTgt.sbsSpeed,								// Ŀ���ٶ�(m/s)
		mCOFlightStatus.flightMemCnt ?
		mCOFlightStatus.memFlightStatus[0].groundSpeed :	// �ѻ��ٶ�(m/s)
		0,
		mCOFlightStatus.flightMemCnt ?
		sqrt( POW2(mACFlightStatus.alt - mCOFlightStatus.memFlightStatus[0].alt) +
		POW2((mACFlightStatus.lon - mCOFlightStatus.memFlightStatus[0].lon) * dis2Lons) +
		POW2((mACFlightStatus.lat - mCOFlightStatus.memFlightStatus[0].lat) * dis2Lats) ) :
		MAX_DIS,											// �ѻ�����
		mCOFlightStatus.flightMemCnt ?
		mACFlightStatus.alt - mCOFlightStatus.memFlightStatus[0].alt :
		MAX_DIS,											// �ѻ��߶Ȳ�
		getTdAngle(mACFlightStatus.velNWU, mCOFlightStatus.memFlightStatus[0].velNWU),
															// �ѻ��ٶ�ʸ���н�
		mACFlightStatus.groundSpeed,						// �����ٶ�
		getTdAngle(mACFlightStatus.velNWU, nearestTgt.velNWU),
															// �л��ٶ�ʸ���н�
		mACMslWarning.mslCnt > 0,							// �Ƿ񱻵�������
		mACMSLInGuide.mslCnt > 0,							// �Ƿ����Ƶ�
		mACFlightStatus.remainWpnNum,						// ʣ��������
		GetTgtSum(),										// ɨ�赽�ĵл�����
		mACMslWarning.mslCnt,								// �����Լ��ĵ�������
		mACMSLInGuide.mslCnt + nCoMslCnt,					// �����ҷ���������
		OutputReward()
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
	}
	// �жϵз��ɻ�����������������״ֵ̬1
	else {
		g_flight_state = 1;
	}
}

int MyStrategy::OutputReward()
{
	int reward = 0;

	// �з��ɻ�����������������һ�ܵл���
	if(g_flight_state == 1)
	{
		// ����״̬����
		g_flight_state = 0;
		reward += 250;
	}
	// �ѷ��ɻ������������������ѷ��ɻ������䣩
	if(g_flight_state == 2)
	{
		// ����״̬����
		g_flight_state = 0;
		reward -= 250;
	}
	// ��������в��������û�зɻ�������ʱ��reward���ҷ����з�����������
	if (g_cnt_state < mACMslWarning.mslCnt && g_flight_state == 0 && mACFlightStatus.flightID == mACMslWarning.flightID)
	{
		g_cnt_state++;
		reward -= 5;
	}
	// ��������в�����ٲ���û�зɻ�׹��ʱ��reward���ҷ����ѵз�������
	if (g_cnt_state > mACMslWarning.mslCnt && g_flight_state == 0 && mACFlightStatus.flightID == mACMslWarning.flightID)
	{
		g_cnt_state--;
		reward += 10;
	}
	// ����鵽�л���������ҷ���õз���Ұ��
	if (g_enmy_state < mACRdrTarget.tgtCnt && g_flight_state == 0)
	{
		g_enmy_state++;
		reward += 2;
	}
	// ��쵽�ĵл��������٣��ҷ���ʧ�з���Ұ��
	if (g_enmy_state > mACRdrTarget.tgtCnt && g_flight_state == 0)
	{
		g_enmy_state--;
		reward -= 2;
	}
	// �ɻ����䵼��
	if (g_launch_state < mTeamMSLLaunched.mslCnt && g_flight_state == 0 && mTeamMSLLaunched.trajectoryInfos[0].launchFlightID == mACFlightStatus.flightID)
	{
		g_launch_state++;
		reward -= 10;
	}
	// �ҷ����������з�һ�ܷɻ�
	if (g_guide_state < mACMSLInGuide.mslCnt && g_flight_state == 0 && mACMSLInGuide.guideFlightID == mACFlightStatus.flightID)
	{
		g_guide_state++;
		reward += 5;
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

	const double dis2Lons = 160000 / (mPKConfig.RightUpLon - mPKConfig.LeftDownLon);
	const double dis2Lats = 80000 / (mPKConfig.RightUpLat - mPKConfig.LeftDownLat);

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
//--------------------------------------
//ս��������

// ƫ���Ƶ�
/*void MyStrategy::DoTacHeadGuide()
{
	ACAI::FlyControlCmd outputData;
	memset(&outputData, 0, sizeof(outputData));
	outputData.executePlaneID = mACFlightStatus.flightID; ///< Ŀ�ķɻ����
	outputData.altCtrlCmd = true;        ///< �߶ȱ���ָ��
	outputData.headCtrlCmd = true;       ///< ���򱣳�ָ��
	outputData.speedCtrlCmd = true;      ///< �ٶȱ���ָ��
	outputData.desireAlt = 2000;       ///< �����߶�(m)
	outputData.desireHead = mACMSLInGuide.guideInfos[0].mslGuideAz+35*PI/180;///< ������·����(rad)
	outputData.desireSpeed = 1000;///< ������·�ٶ�(m/s)
	outputData._cmdCnt = mACFlightStatus.timeCounter;   ///< ָ�����
	sendFlyControlCmd(outputData);
}*/