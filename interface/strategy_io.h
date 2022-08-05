/*******************************************************************************
 * \file        strategy_io.h
 * \authors     weiqm<4146>, linwx<4204>
 * \copyright   Copyright (c) 2021, 611.CADI All rights reserved.
 *******************************************************************************/

#ifndef STRATEGY_IO_H
#define STRATEGY_IO_H

/// STRATEGY_IO_VERSION is (major << 16) + (minor << 8) + patch.
#define STRATEGY_IO_VERSION 0x020000
#define STRATEGY_IO_VERSION_STR "2.00"

namespace ACAI {
//------------------------------------------

/// \enum �ɻ���Ӫ
enum FLIGHT_TEAM {
    V_FLIGHT_TEAM_RED,    ///< �췽��Ӫ
    V_FLIGHT_TEAM_BLUE    ///< ������Ӫ
};

/// \enum �ɻ���ɫ
enum FLIGHT_ROLE {
    V_FLIGHT_ROLE_FREE = 0, ///< ����
    V_FLIGHT_ROLE_LEAD,     ///< ����
    V_FLIGHT_ROLE_WING,     ///< �Ż�
};

/// \enum ���ܿ���״̬
enum FUNC_STATUS {
    V_FUNC_STATUS_OFF = 0, ///< ���ܹ�
    V_FUNC_STATUS_ON = 1,  ///< ���ܿ�
};


enum { MAX_FLIGHT_NUM = 32 }; ///< ���ɻ���
enum { MAX_WPN_STA_NUM = 10 }; ///< ��������ҵ���
enum { MAX_RDR_TGT_NUM = 31 }; ///< ����״�Ŀ����
enum { MAX_EWS_TGT_NUM = 31 }; ///< ������սĿ����
enum { MAX_MSL_NUM = 80 }; ///< ��󵼵���
enum { MAX_GUIDE_MSL_NUM = 8 }; ///< ����Ƶ�������
enum { MAX_SHOOT_LIST_NUM = 31}; ///< �������б���
enum { MAX_ITDB_DATA_LEN = 1200 }; ///< ����������ݰ����ݳ���
enum { MAX_EVENT_LOG_STRING_LEN = 100 }; ///< �����Ϣ��־�ַ�������

//------------------------------------------

/// \struct ����������Ϣ
struct PKConfig
{
    double	MinFlyHeight	;//��С���и߶�(m)
	double  MaxFlyHeight	;//�����и߶�(m)
	double  LeftDownLon		;//�������½Ǿ���(rad)
	double  LeftDownLat		;//�������½�γ��(rad)
	double  RightUpLon		;//�������ϽǾ���(rad)
	double	RightUpLat		;//�������Ͻ�γ��(rad)
	double  RedMissionLon	;//�췽����Ŀ��㾭��(rad)
	double  RedMissionLat	;//�췽����Ŀ���γ��(rad)
	double  BlueMissionLon	;//��������Ŀ��㾭��(rad)
	double  BlueMissionLat	;//��������Ŀ���γ��(rad)
};

//------------------------------------------

/// \struct �ɻ�״̬����
struct ACFlightStatus
{
    unsigned long timeCounter;            ///< ʱ��(ms)
	unsigned int simuSpeed;               ///< ���汶��
    unsigned int flightID;                ///< �ɻ����(Ψһ��ʶ)
    FLIGHT_TEAM flightTeam;               ///< �ɻ���Ӫ \sa FLIGHT_TEAM
    FLIGHT_ROLE flightRole;               ///< �ɻ���ɫ \sa FLIGHT_ROLE
    double lon;                           ///< ����(rad)
    double lat;                           ///< γ��(rad)
    double alt;                           ///< ���θ߶�(m)
    double heading;                       ///< �����(rad ˳ʱ��Ϊ��)
    double pitch;                         ///< ������(rad ��Ϊ��)
    double roll;                          ///< �����(rad ��Ϊ��)
    double groundSpeed;                   ///< ����(m/s)
    double machSpeed;                     ///< �����(mach)
    double velNWU[3];                     ///< ����ϵ�ٶ�ʸ��(m/s)
    double overLoad;                      ///< ��ǰ����G
    bool isLinkSysAvailable;              ///< ����������״̬(�Ƿ����)
    bool isRdrSysAvailable;               ///< �״����״̬(�Ƿ����)
    bool isEwsSysAvailable;               ///< ����ս����״̬(�Ƿ����)
    bool isLinkSysOpen;                   ///< ����������״̬
    bool isRdrSysOpen;                    ///< �״￪��״̬
    bool isEwsSysOpen;                    ///< ����ս����״̬
    double maxRdrRange;                   ///< �״�������þ���(m)
    double maxRdrAzAngle;                 ///< �״��ܽ�(rad)
    unsigned int remainGuideNum;          ///< ʣ���Ƶ���Դ
    unsigned int remainWpnNum;            ///< ʣ����������
};

/// \struct �״�̽��Ŀ��
struct ACRdrTarget
{
    unsigned long timeCounter; ///< ʱ��
    unsigned int flightID;     ///< �ɻ����(Ψһ��ʶ)
    unsigned int tgtCnt;       ///< Ŀ������(<=MAX_RDR_TGT_NUM)
    /// \struct �״�Ŀ����Ϣ
    struct RdrTgtInfo {
        unsigned int tgtID; ///< Ŀ����(Ŀ��ɻ����)
        bool rngValid;      ///< ������Ч��
        bool velValid;      ///< �ٶ���Ч��
        bool angValid;      ///< �Ƕ���Ч��
        double lon;         ///< ����(rad)
        double lat;         ///< γ��(rad)
        double alt;         ///< ���θ߶�(m)
        double slantRange;  ///< Ŀ��б��(m)
        double velNWU[3];   ///< ����ϵ�ٶ�ʸ��(m/s)
        double sbsSpeed;    ///< Ŀ���ٶȴ�С(m/s)
        double machSpeed;   ///< Ŀ�������(mac)
        double heading;     ///< Ŀ�꺽���(rad ˳ʱ��Ϊ��)
        double aspect;      ///< Ŀ������(rad)
        double azGeo;       ///< Ŀ�����ϵ��λ(rad ��Ϊ��)
        double elGeo;       ///< Ŀ�����ϵ����(rad ��Ϊ��)
        double azBody;      ///< Ŀ�����ϵ��λ(rad ��Ϊ��)
        double elBody;      ///< Ŀ�����ϵ����(rad ��Ϊ��)
    } tgtInfos[MAX_RDR_TGT_NUM]; ///< Ŀ����Ϣ
};

/// \struct ����ս̽��Ŀ��
struct ACEwsTarget
{
    unsigned long timeCounter; ///< ʱ��
    unsigned int flightID;     ///< �ɻ����(Ψһ��ʶ)
    unsigned int tgtCnt;       ///< Ŀ������(<=MAX_EWS_TGT_NUM)
    /// \struct ����սĿ����Ϣ
    struct EwsTgtInfo {
        unsigned int tgtID; ///< Ŀ����(Ŀ��ķɻ����)
        bool angValid;      ///< �Ƕ���Ч��
        double azGeo;       ///< Ŀ�����ϵ��λ(rad ��Ϊ��)
        double azBody;      ///< Ŀ�����ϵ��λ(rad ��Ϊ��)
    } tgtInfos[MAX_EWS_TGT_NUM]; ///< Ŀ����Ϣ
};

/// \struct ������в�澯
struct ACMslWarning
{
    unsigned long timeCounter; ///< ʱ��
    unsigned int flightID;     ///< �ɻ����(Ψһ��ʶ)
    unsigned int mslCnt;       ///< ��������(<=MAX_MSL_NUM)
    /// \struct ��в������Ϣ
    struct ThreatMslInfo {
        bool angValid; ///< �Ƕ���Ч��
        double azGeo;  ///< ��������ϵ��λ(rad ��Ϊ��)
        double azBody; ///< ��������ϵ��λ(rad ��Ϊ��)
    } threatInfos[MAX_MSL_NUM]; ///< ��в������Ϣ
};

/// \struct �����ذ���
struct ACFCCStatus
{
    unsigned long timeCounter; ///< ʱ��
    unsigned int flightID;     ///< �ɻ����(Ψһ��ʶ)
    unsigned int tgtCnt;       ///< Ŀ������(<=MAX_SHOOT_LIST_NUM)
    /// \struct ��ذ�����Ϣ
    struct FccEnvInfo {
        unsigned int tgtID; ///< Ŀ����
        bool isNTSTgt;      ///< �Ƿ�NTSĿ��
        bool shootFlag;     ///< ������ʾ��
        double slantRange;  ///< Ŀ��б��(m)
        double aspect;      ///< Ŀ������(rad)
        double Rmax;        ///< ��������Rmax(m)�����赼�������Ŀ�겻��������������󹥻����룩
        double Rtr;         ///< ��������Rtr(m)�����赼�������Ŀ��ˮƽ��β����������󹥻����룩
        bool APoleValid;    ///< A����Ч�ԣ�A����ʾ��������Ԥ���ܹ��ػ�Ŀ�꣩
        bool FPoleValid;    ///< F����Ч�ԣ�F����ʾ��������Ԥ���ܹ�����Ŀ�꣩
        double APoleTime;   ///< A��ʱ�䣨Ԥ�ƽػ�Ŀ���ʱ�䣩
        double FPoleTime;   ///< F��ʱ�䣨Ԥ������Ŀ���ʱ�䣩
        double APoleRange;  ///< A�����루Ԥ�ƽػ�Ŀ��ʱ�̵��ػ���Ŀ����룩
        double FPoleRange;  ///< F�����루Ԥ������Ŀ��ʱ�̵��ػ���Ŀ����룩
        double FoeRmax;     ///< �а���Rmax(m)������з���������󱾻�����������������󹥻����룩
        double FoeRtr;      ///< �а���Rtr(m)������з���������󱾻�����������������󹥻����룩
    } envInfos[MAX_SHOOT_LIST_NUM]; ///< ��ذ�����Ϣ
};

/// \struct �Ƶ�������Ϣ
struct ACMSLInGuide
{
    unsigned long timeCounter;  ///< ʱ��
    unsigned long lostCounter;  ///< ��֡����
    unsigned int guideFlightID; ///< �Ƶ������
    unsigned int mslCnt;        ///< ��������(<=MAX_GUIDE_MSL_NUM)
    /// \struct �Ƶ�������Ϣ
    struct GuideMslInfo {
        unsigned int launchFlightID; ///< ��������
        unsigned int mslStaIndex;    ///< �����ҵ��±�
        unsigned int mslLockedTgtID; ///< ��������Ŀ����
        unsigned int mslLostCounter; ///< ������ʧ����
        double mslGuideRange;        ///< �����Ƶ�����(m)
        double mslGuideAz;           ///< �����Ƶ���λ(rad)
        bool isGuideCmdValid;        ///< �Ƶ�ָ����Ч��
    } guideInfos[MAX_GUIDE_MSL_NUM]; ///< �Ƶ�������Ϣ
};

/// \struct ��ӷ��䵼��ʵʱ��Ϣ
struct TeamMSLLaunched
{
    unsigned long timeCounter; ///< ʱ��
    unsigned int mslCnt;       ///< ��������(<=MAX_MSL_NUM)
    /// \struct ����������Ϣ
    struct MslTrajectoryInfo {
        unsigned int launchFlightID; ///< ��������
        unsigned int mslStaIndex;    ///< �����ҵ��±�
        unsigned int mslLockedTgtID; ///< ��������Ŀ����
        double lon;                  ///< ��������(rad)
        double lat;                  ///< ����γ��(rad)
        double alt;                  ///< �������θ߶�(m)
        double velNWU[3];            ///< ����ϵ�ٶ�ʸ��(m/s)
        double flyTime;              ///< �����ѷ�ʱ��(s)
        double remainTime;           ///< ����ʣ�����ʱ��(s)
        bool isSeekerCaptured;       ///< ����ͷ�Ƿ�ػ�
        bool APoleValid;             ///< A����Ч��
        bool FPoleValid;             ///< F����Ч��
        double APoleTime;            ///< A��ʱ��(s)
        double FPoleTime;            ///< F��ʱ��(s)
        double APoleRange;           ///< A������(m)
        double FPoleRange;           ///< F������(m)
    } trajectoryInfos[MAX_MSL_NUM]; ///< ����������Ϣ
};

//------------------------------------------

/// \struct ��ӳ�Ա�ɻ�״̬����
struct COFlightStatus
{
    unsigned int flightMemCnt; ///< �ɻ���Ա��
    ACFlightStatus memFlightStatus[MAX_FLIGHT_NUM]; ///< ��Ա�ɻ�״̬������Ϣ
};

/// \struct ��ӳ�Ա�״�̽��Ŀ��
struct CORdrTarget
{
    unsigned int flightMemCnt; ///< �ɻ���Ա��
    ACRdrTarget memRdrTarget[MAX_FLIGHT_NUM]; ///< ��Ա�״�̽��Ŀ����Ϣ
};

/// \struct ��ӳ�Ա����ս̽��Ŀ��
struct COEwsTarget
{
    unsigned int flightMemCnt; ///< �ɻ���Ա��
    ACEwsTarget memEwsTarget[MAX_FLIGHT_NUM]; ///< ��Ա����ս̽��Ŀ����Ϣ
};

/// \struct ��ӳ�Ա����ս�����澯
struct COMslWarning
{
    unsigned int flightMemCnt; ///< �ɻ���Ա��
    ACMslWarning memMslWarning[MAX_FLIGHT_NUM]; ///< ��Ա����ս�����澯��Ϣ
};

/// \struct ��ӳ�Ա�����ذ���
struct COFCCStatus
{
    unsigned int flightMemCnt; ///< �ɻ���Ա��
    ACFCCStatus memFCCStatus[MAX_FLIGHT_NUM]; ///< ��Ա�����ذ�����Ϣ
};

/// \struct ��ӳ�Ա�Ƶ�������Ϣ
struct COMSLInGuide
{
    unsigned int flightMemCnt; ///< �ɻ���Ա��
    ACMSLInGuide memMSLInGuide[MAX_FLIGHT_NUM]; ///< ��Ա�Ƶ�������Ϣ��Ϣ
};

//------------------------------------------

/// \struct ���п���ָ��
struct FlyControlCmd
{
	unsigned int executePlaneID; ///< Ŀ�ķɻ����
    bool velCtrlCmd;        ///< �ٶ�ʸ��ָ��
    bool navCtrlCmd;        ///< ��·����ָ��
    bool altCtrlCmd;        ///< �߶ȱ���ָ��
    bool headCtrlCmd;       ///< ���򱣳�ָ��
    bool speedCtrlCmd;      ///< �ٶȱ���ָ��
    double desireVelNWU[3]; ///< �����ٶ�ʸ��(m/s)
    double desireNavLon;    ///< ������·����(rad)
    double desireNavLat;    ///< ������·γ��(rad)
    double desireNavAlt;    ///< ������·�߶�(m)
    double desireAlt;       ///< �����߶�(m)
    double desireHead;      ///< ������·����(rad)
    double desireSpeed;     ///< ������·�ٶ�(m/s)
    unsigned int _cmdCnt;   ///< ָ�����
};

/// \struct ����������ָ��
struct SnsControlCmd
{
    unsigned int executePlaneID; ///< Ŀ�ķɻ����
    FUNC_STATUS rdrOpenCmd;      ///< �״￪��ָ�� \sa FUNC_STATUS
    FUNC_STATUS ewsOpenCmd;      ///< ����ս����ָ�� \sa FUNC_STATUS
    FUNC_STATUS linkOpenCmd;     ///< ����������ָ�� \sa FUNC_STATUS
    unsigned int _cmdCnt;        ///< ָ�����
};

/// \struct ��������ָ��
struct WpnControlCmd
{
    unsigned int launchPlaneID; ///< ��������
    unsigned int guidePlaneID;  ///< �Ƶ������
    unsigned int mslLockTgtID;  ///< ��������Ŀ����
    unsigned int _cmdCnt;       ///< ָ�����
};

//------------------------------------------

/// \struct ����ڲ����ݰ�
struct InTeamDataBag
{
    unsigned int DST_PLANE_ID;  ///< ���շɻ����
    unsigned int DATA_BAG_TYPE; ///< ���ݰ�����
    char DATA[MAX_ITDB_DATA_LEN]; ///< ���ݰ�
};

//------------------------------------------

/// \struct �ɻ��ӿ��¼���־�ṹ��
struct EventLog
{
    char EventDes[MAX_EVENT_LOG_STRING_LEN]; ///< �����ĵ�
};

//------------------------------------------

/// \typedef �ص�����_���п���
typedef bool (* P_CALL_BACK_FLY_CTRL) (const FlyControlCmd&);

/// \typedef �ص�����_����������
typedef bool (* P_CALL_BACK_SNS_CTRL) (const SnsControlCmd&);

/// \typedef �ص�����_��������
typedef bool (* P_CALL_BACK_WPN_CTRL) (const WpnControlCmd&);

/// \typedef �ص�����_����ڲ����ݰ�����
typedef bool (* P_CALL_BACK_SEND_ITDB) (const InTeamDataBag&);

/// \typedef �ص�����_�¼���־��¼
typedef bool (* P_CALL_BACK_EVENT_LOG) (const EventLog&);

//------------------------------------------
}

#endif // STRATEGY_IO_H
