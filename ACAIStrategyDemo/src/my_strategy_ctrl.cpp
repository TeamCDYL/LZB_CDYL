#include "my_strategy.h"
#include "string.h"

int g_flight_state;	//�ɻ����״̬
int g_cnt_state;		//������в״̬
int g_enmy_state;	//�л�����״̬
int g_launch_state;	//�ҷ����䵼��״̬
int g_guide_state;	//�ҷ��Ƶ�����״̬

#define LEAD_STATE "state1.csv"
#define WING_STATE "state2.csv"
#define LEAD_ACTION "action1.csv"
#define WING_ACTION "action2.csv"
#define LEAD_REWARD "reward1.csv"
#define WING_REWARD "reward2.csv"
const char * ActionTitle = "action first index, action second index\n";
const char * StateTitle = "[time counter], distance to enemy, alt distance to enemy, "
						"direction to enemy, enemy speed, friend speed, "
						"distance to friend, alt distance to friend, speed direct to friend, "
						"self speed, speed direct to enemy, is locked by msl, "
						"is in guide, remaining msl, enemy in rdr, "
						"n msl locked, our n msl\n";
const char * RewardTitle = "reward\n";
static int timecounter=0;
static int testcounter=0;

void MyStrategy::onPKStart(const ACAI::PKConfig &pkConfig)
{
    memcpy(&mPKConfig, &pkConfig, sizeof(mPKConfig));
    // TODO : �˴�����û�������Ӧ������ʼ
	if (mACFlightStatus.flightRole == ACAI::V_FLIGHT_ROLE_LEAD) {
		FILE *tmp = fopen(LEAD_STATE, "w"); // ����ļ�����
		fprintf(tmp, StateTitle);			// �ļ���ͷ
		fclose(tmp);
		tmp = fopen(LEAD_ACTION, "w");
		fprintf(tmp, ActionTitle);
		fclose(tmp);
		tmp = fopen(LEAD_REWARD, "w");
		fprintf(tmp, RewardTitle);
		fclose(tmp);
		PrintStatus(LEAD_STATE);
	} else {
		FILE *tmp = fopen(WING_STATE, "w"); // ����ļ�����
		fprintf(tmp, StateTitle);			// �ļ���ͷ
		fclose(tmp);
		tmp = fopen(WING_ACTION, "w");
		fprintf(tmp, ActionTitle);
		fclose(tmp);
		tmp = fopen(WING_REWARD, "w");
		fprintf(tmp, RewardTitle);
		fclose(tmp);
		PrintStatus(WING_STATE);
	}
	FILE *tmp = fopen("false", "w"); // ��־��ս��ʼ
	fclose(tmp);
	remove("true");
}

void MyStrategy::onPKEnd()
{
    // TODO : �˴�����û�������Ӧ��������
	initData();
	FILE *tmp = fopen("true", "w"); // ��־��ս����
	fclose(tmp);
	remove("false");
}

void MyStrategy::onPKOut(unsigned int flightID)
{
	// TODO : �˴�����û�������Ӧ�ѷ����ߵз��ɻ������л�����˳������¼�
}

void MyStrategy::backGround()
{
    // TODO : �˴�����û�����ִ�к�̨����
}

void MyStrategy::timeSlice20()
{
    // TODO : �˴�����û�����ִ��20ms��������
}

bool inRange(double angle, double range) {
	return (angle <= range && angle >= -range) ||
		(PI - angle <= range && PI - angle >= -range);
}

void MyStrategy::timeSlice40()
{
	timecounter++;
	testcounter++;
	if ((timecounter%9)==0)
	{
		// TODO : �˴�����û�����ִ��40ms��������
		ACAI::EventLog outputData;
		memset(&outputData, 0, sizeof(outputData));
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


#ifdef DEEP_LEARNING	// ���ѧϰʱ��ȡ����
		readAction(); 
#endif

#ifndef DEEP_LEARNING // ר��ģʽʱִ�ж���
		//����ս��
		static int mslWarningStartTime = 0;
		if (mACMslWarning.mslCnt > 0) {	
			if (mACFlightStatus.timeCounter - mslWarningStartTime > 3000) {		// �������濪ʼ3�����
				readActionByPro(0, 11);
				//DoTacHeadEvade();
				PrintState();
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
			strcpy(outputData.EventDes, "��з�����ͻΧ");
			logEvent(outputData);
		}
		if (mACRdrTarget.tgtCnt > 0 && mACMslWarning.mslCnt == 0 && mACMSLInGuide.mslCnt > 0) {
			// �ж�׷������ͻ��
			// ���ڵ���ʱ
			if (mACFCCStatus.envInfos[0].FPoleValid || mACFCCStatus.envInfos[0].APoleValid) {
				readActionByPro(1, 1);
				//DoTacWpnShoot();
				PrintState();
				strcpy(outputData.EventDes, "��������");
				logEvent(outputData);
			}
			if (mACFCCStatus.envInfos[1].FPoleValid || mACFCCStatus.envInfos[1].APoleValid) {
				readActionByPro(1, 3);
				//DoTacWpnShoot(1);
				PrintState();
				strcpy(outputData.EventDes,"��������");
				logEvent(outputData);
			} else {
				readActionByPro(0, 2);
				//DoTacToTar();
				PrintState();
				strcpy(outputData.EventDes, "��һ�ܵз��ɻ�����");
				logEvent(outputData);
			}
		}
		if (mACRdrTarget.tgtCnt > 0 && mACMslWarning.mslCnt == 0 && mACMSLInGuide.mslCnt == 0) {
			readActionByPro(0, 4);
			//DoTacAltClimbP60();
			PrintState();
			strcpy(outputData.EventDes, "��60������");
			logEvent(outputData);
			if (mACFlightStatus.alt==4000) {
				readActionByPro(0, 1);
				//DoTacPointAtk();
				PrintState();
				strcpy(outputData.EventDes, "��з�����ͻϮ");
				logEvent(outputData);
			}
		}
#endif
	}


    
}

//--------------------------------------
//��ȡ����
struct Action {
	int fin, sin; // һ��������0�����У�1���������������������嶯��
};

void deal(const char* filename, LPVOID lParam) { // ����ԭ�򲻷���ӵ�MyStrategy����
	FILE* fp = fopen(filename, "r");
	int fin, sin; // һ����������������
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
		if (watch(L".", LEAD_ACTION, deal, &act)) {
			maneuver_i(act.fin, act.sin);
		} else { // �����ڼ��ļ�δ�����ı�
			DoTacHeadEvade();
			return;
		}
		// TODO:�յ�action��ķ���
		PrintStatus(LEAD_STATE);
	} else {
		if (watch(L".", WING_ACTION, deal, &act)) {
			maneuver_i(act.fin, act.sin);
		} else { // �����ڼ��ļ�δ�����ı�
			DoTacHeadEvade();
			return;
		}
		PrintStatus(WING_STATE);
	}
	return;
}

void MyStrategy::readActionByPro(int fin, int sin) { // һ��������
	maneuver_i(fin, sin);
#ifndef DEEP_LEARNING // ר��ģʽʱ�������
	if (mACRdrTarget.tgtCnt > 0) {
		if (mACFlightStatus.flightRole == ACAI::V_FLIGHT_ROLE_LEAD) {
			FILE *fp = fopen(LEAD_ACTION, "a");
			fprintf(fp, "%d, %d\n", fin, sin);
			fclose(fp);
		} else {
			FILE *fp = fopen(WING_ACTION, "a");
			fprintf(fp, "%d, %d\n", fin, sin);
			fclose(fp);
		}
	}
#endif // DEEP_LEARNING
}

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
	if (mACFlightStatus.flightRole == ACAI::V_FLIGHT_ROLE_LEAD) PrintStatus(LEAD_STATE);
	else PrintStatus(WING_STATE);
}

void MyStrategy::PrintStatus(const char * filename) {
	static const double dis2Lons = 160000 / (mPKConfig.RightUpLon - mPKConfig.LeftDownLon);
	static const double dis2Lats = 80000 / (mPKConfig.RightUpLat - mPKConfig.LeftDownLat);
	int nCoMslCnt = 0;
	for (int i = 0; i < mCOMSLInGuide.flightMemCnt; ++i) {
		nCoMslCnt += mCOMSLInGuide.memMSLInGuide[0].mslCnt;
	}
	FILE *fp = fopen(filename, "a");
	fprintf(fp, "[%d], %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %d, %d, %d, %d, %d, %d\n",
		mACFlightStatus.timeCounter,						// [ʱ��]
		mACRdrTarget.tgtInfos[0].slantRange,				// Ŀ�����
		mACFlightStatus.alt - mACRdrTarget.tgtInfos[0].alt, // Ŀ��߶Ȳ�
		mACRdrTarget.tgtInfos[0].azGeo,						// Ŀ�� ����ϵ ��λ��
		mACRdrTarget.tgtInfos[0].sbsSpeed,					// Ŀ���ٶ�(m/s)
		mCOFlightStatus.flightMemCnt ?
		mCOFlightStatus.memFlightStatus[0].groundSpeed :	// �ѻ��ٶ�(m/s)
		0,
		mCOFlightStatus.flightMemCnt ?
		sqrt( POW2(mACFlightStatus.alt - mCOFlightStatus.memFlightStatus[0].alt) +
		POW2((mACFlightStatus.lon - mCOFlightStatus.memFlightStatus[0].lon) * dis2Lons) +
		POW2((mACFlightStatus.lat - mCOFlightStatus.memFlightStatus[0].lat) * dis2Lats) ) :
		DBL_MAX,											// �ѻ�����
		mCOFlightStatus.flightMemCnt ?
		mACFlightStatus.alt - mCOFlightStatus.memFlightStatus[0].alt :
		DBL_MAX,											// �ѻ��߶Ȳ�
		getTdAngle(mACFlightStatus.velNWU, mCOFlightStatus.memFlightStatus[0].velNWU),
															// �ѻ��ٶ�ʸ���н�
		mACFlightStatus.groundSpeed,						// �����ٶ�
		getTdAngle(mACFlightStatus.velNWU, mACRdrTarget.tgtInfos[0].velNWU),
															// �л��ٶ�ʸ���н�
		mACMslWarning.mslCnt > 0,							// �Ƿ񱻵�������
		mACMSLInGuide.mslCnt > 0,							// �Ƿ����Ƶ�
		mACFlightStatus.remainWpnNum,						// ʣ��������
		mACRdrTarget.tgtCnt,								// ɨ�赽�ĵл�����
		mACMslWarning.mslCnt,								// �����Լ��ĵ�������
		mACMSLInGuide.mslCnt + nCoMslCnt					// �����ҷ���������
		);
	fclose(fp);
}

//--------------------------------------
//����


//--------------------------------------
//����������
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
	PrintReward();
}
//��з����߷���
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
//��һ�ܵл�����
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
//+30�ȼ�������
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
//+60�ȼ�������
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
//-30����Ǳ
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
//-60����Ǳ
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
//���λ���
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
//��תƫ��30
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
//��תƫ��60
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
//��תƫ��30
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
//��תƫ��60
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
//��ͷ
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

//��ͷ��30����Ǳ
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
//��ͷ��60����Ǳ
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
//-30����Ǳ
//��ת��
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
//��ת��
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
//��ǰ����
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

//--------------------------------------
//ս��������
// ��������
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

//�л��Ƶ�������������
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
//�ػ�
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
//״̬����
void MyStrategy::SetFlightState(unsigned int flightID)
{
	// �ж��ѷ������������ɻ�����������������״ֵ̬2
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

	// �жϵз��ɻ�����������������״ֵ̬1
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
//���ͺ���
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
//��ӡ������ֵ
void MyStrategy::PrintReward() {
	int numberReward;
	FILE* fp;
	if(mACFlightStatus.flightRole == ACAI::V_FLIGHT_ROLE_LEAD) {
		fp = fopen(LEAD_REWARD, "a");
		numberReward = OutputReward();
		fprintf(fp,"%d\n",numberReward);
		fclose(fp);
	} else {
		fp = fopen(WING_REWARD, "a");
		numberReward = OutputReward();
		fprintf(fp,"%d\n",numberReward);
		fclose(fp);
	}
}