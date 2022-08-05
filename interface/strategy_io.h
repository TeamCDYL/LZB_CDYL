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

/// \enum 飞机阵营
enum FLIGHT_TEAM {
    V_FLIGHT_TEAM_RED,    ///< 红方阵营
    V_FLIGHT_TEAM_BLUE    ///< 蓝方阵营
};

/// \enum 飞机角色
enum FLIGHT_ROLE {
    V_FLIGHT_ROLE_FREE = 0, ///< 自由
    V_FLIGHT_ROLE_LEAD,     ///< 长机
    V_FLIGHT_ROLE_WING,     ///< 僚机
};

/// \enum 功能开关状态
enum FUNC_STATUS {
    V_FUNC_STATUS_OFF = 0, ///< 功能关
    V_FUNC_STATUS_ON = 1,  ///< 功能开
};


enum { MAX_FLIGHT_NUM = 32 }; ///< 最大飞机数
enum { MAX_WPN_STA_NUM = 10 }; ///< 最大武器挂点数
enum { MAX_RDR_TGT_NUM = 31 }; ///< 最大雷达目标数
enum { MAX_EWS_TGT_NUM = 31 }; ///< 最大电子战目标数
enum { MAX_MSL_NUM = 80 }; ///< 最大导弹数
enum { MAX_GUIDE_MSL_NUM = 8 }; ///< 最大制导导弹数
enum { MAX_SHOOT_LIST_NUM = 31}; ///< 最大射击列表数
enum { MAX_ITDB_DATA_LEN = 1200 }; ///< 最大编队内数据包数据长度
enum { MAX_EVENT_LOG_STRING_LEN = 100 }; ///< 最大消息日志字符串长度

//------------------------------------------

/// \struct 比赛配置信息
struct PKConfig
{
    double	MinFlyHeight	;//最小飞行高度(m)
	double  MaxFlyHeight	;//最大飞行高度(m)
	double  LeftDownLon		;//矩形左下角经度(rad)
	double  LeftDownLat		;//矩形左下角纬度(rad)
	double  RightUpLon		;//矩形右上角经度(rad)
	double	RightUpLat		;//矩形右上角纬度(rad)
	double  RedMissionLon	;//红方任务目标点经度(rad)
	double  RedMissionLat	;//红方任务目标点纬度(rad)
	double  BlueMissionLon	;//蓝方任务目标点经度(rad)
	double  BlueMissionLat	;//蓝方任务目标点纬度(rad)
};

//------------------------------------------

/// \struct 飞机状态数据
struct ACFlightStatus
{
    unsigned long timeCounter;            ///< 时标(ms)
	unsigned int simuSpeed;               ///< 仿真倍速
    unsigned int flightID;                ///< 飞机编号(唯一标识)
    FLIGHT_TEAM flightTeam;               ///< 飞机阵营 \sa FLIGHT_TEAM
    FLIGHT_ROLE flightRole;               ///< 飞机角色 \sa FLIGHT_ROLE
    double lon;                           ///< 经度(rad)
    double lat;                           ///< 纬度(rad)
    double alt;                           ///< 海拔高度(m)
    double heading;                       ///< 航向角(rad 顺时针为正)
    double pitch;                         ///< 俯仰角(rad 上为正)
    double roll;                          ///< 横滚角(rad 右为正)
    double groundSpeed;                   ///< 地速(m/s)
    double machSpeed;                     ///< 马赫数(mach)
    double velNWU[3];                     ///< 地理系速度矢量(m/s)
    double overLoad;                      ///< 当前过载G
    bool isLinkSysAvailable;              ///< 数据链挂载状态(是否可用)
    bool isRdrSysAvailable;               ///< 雷达挂载状态(是否可用)
    bool isEwsSysAvailable;               ///< 电子战挂载状态(是否可用)
    bool isLinkSysOpen;                   ///< 数据链开机状态
    bool isRdrSysOpen;                    ///< 雷达开机状态
    bool isEwsSysOpen;                    ///< 电子战开机状态
    double maxRdrRange;                   ///< 雷达最大作用距离(m)
    double maxRdrAzAngle;                 ///< 雷达框架角(rad)
    unsigned int remainGuideNum;          ///< 剩余制导资源
    unsigned int remainWpnNum;            ///< 剩余武器数量
};

/// \struct 雷达探测目标
struct ACRdrTarget
{
    unsigned long timeCounter; ///< 时标
    unsigned int flightID;     ///< 飞机编号(唯一标识)
    unsigned int tgtCnt;       ///< 目标数量(<=MAX_RDR_TGT_NUM)
    /// \struct 雷达目标信息
    struct RdrTgtInfo {
        unsigned int tgtID; ///< 目标编号(目标飞机编号)
        bool rngValid;      ///< 距离有效性
        bool velValid;      ///< 速度有效性
        bool angValid;      ///< 角度有效性
        double lon;         ///< 经度(rad)
        double lat;         ///< 纬度(rad)
        double alt;         ///< 海拔高度(m)
        double slantRange;  ///< 目标斜距(m)
        double velNWU[3];   ///< 地理系速度矢量(m/s)
        double sbsSpeed;    ///< 目标速度大小(m/s)
        double machSpeed;   ///< 目标马赫速(mac)
        double heading;     ///< 目标航向角(rad 顺时针为正)
        double aspect;      ///< 目标进入角(rad)
        double azGeo;       ///< 目标地理系方位(rad 右为正)
        double elGeo;       ///< 目标地理系俯仰(rad 上为正)
        double azBody;      ///< 目标机体系方位(rad 右为正)
        double elBody;      ///< 目标机体系俯仰(rad 上为正)
    } tgtInfos[MAX_RDR_TGT_NUM]; ///< 目标信息
};

/// \struct 电子战探测目标
struct ACEwsTarget
{
    unsigned long timeCounter; ///< 时标
    unsigned int flightID;     ///< 飞机编号(唯一标识)
    unsigned int tgtCnt;       ///< 目标数量(<=MAX_EWS_TGT_NUM)
    /// \struct 电子战目标信息
    struct EwsTgtInfo {
        unsigned int tgtID; ///< 目标编号(目标的飞机编号)
        bool angValid;      ///< 角度有效性
        double azGeo;       ///< 目标地理系方位(rad 右为正)
        double azBody;      ///< 目标机体系方位(rad 右为正)
    } tgtInfos[MAX_EWS_TGT_NUM]; ///< 目标信息
};

/// \struct 导弹威胁告警
struct ACMslWarning
{
    unsigned long timeCounter; ///< 时标
    unsigned int flightID;     ///< 飞机编号(唯一标识)
    unsigned int mslCnt;       ///< 导弹数量(<=MAX_MSL_NUM)
    /// \struct 威胁导弹信息
    struct ThreatMslInfo {
        bool angValid; ///< 角度有效性
        double azGeo;  ///< 导弹地理系方位(rad 右为正)
        double azBody; ///< 导弹机体系方位(rad 右为正)
    } threatInfos[MAX_MSL_NUM]; ///< 威胁导弹信息
};

/// \struct 发射火控包线
struct ACFCCStatus
{
    unsigned long timeCounter; ///< 时标
    unsigned int flightID;     ///< 飞机编号(唯一标识)
    unsigned int tgtCnt;       ///< 目标数量(<=MAX_SHOOT_LIST_NUM)
    /// \struct 火控包线信息
    struct FccEnvInfo {
        unsigned int tgtID; ///< 目标编号
        bool isNTSTgt;      ///< 是否NTS目标
        bool shootFlag;     ///< 发射提示符
        double slantRange;  ///< 目标斜距(m)
        double aspect;      ///< 目标进入角(rad)
        double Rmax;        ///< 攻击包线Rmax(m)（假设导弹发射后目标不机动，导弹的最大攻击距离）
        double Rtr;         ///< 攻击包线Rtr(m)（假设导弹发射后目标水平置尾，导弹的最大攻击距离）
        bool APoleValid;    ///< A极有效性（A极表示导弹发射预计能够截获目标）
        bool FPoleValid;    ///< F极有效性（F极表示导弹发射预计能够命中目标）
        double APoleTime;   ///< A极时间（预计截获目标的时间）
        double FPoleTime;   ///< F极时间（预计命中目标的时间）
        double APoleRange;  ///< A极距离（预计截获目标时刻的载机和目标距离）
        double FPoleRange;  ///< F极距离（预计命中目标时刻的载机和目标距离）
        double FoeRmax;     ///< 敌包线Rmax(m)（假设敌方导弹发射后本机不机动，导弹的最大攻击距离）
        double FoeRtr;      ///< 敌包线Rtr(m)（假设敌方导弹发射后本机不机动，导弹的最大攻击距离）
    } envInfos[MAX_SHOOT_LIST_NUM]; ///< 火控包线信息
};

/// \struct 制导武器信息
struct ACMSLInGuide
{
    unsigned long timeCounter;  ///< 时标
    unsigned long lostCounter;  ///< 丢帧计数
    unsigned int guideFlightID; ///< 制导机编号
    unsigned int mslCnt;        ///< 导弹数量(<=MAX_GUIDE_MSL_NUM)
    /// \struct 制导武器信息
    struct GuideMslInfo {
        unsigned int launchFlightID; ///< 发射机编号
        unsigned int mslStaIndex;    ///< 导弹挂点下标
        unsigned int mslLockedTgtID; ///< 导弹锁定目标编号
        unsigned int mslLostCounter; ///< 导弹丢失计数
        double mslGuideRange;        ///< 导弹制导距离(m)
        double mslGuideAz;           ///< 导弹制导方位(rad)
        bool isGuideCmdValid;        ///< 制导指令有效性
    } guideInfos[MAX_GUIDE_MSL_NUM]; ///< 制导武器信息
};

/// \struct 编队发射导弹实时信息
struct TeamMSLLaunched
{
    unsigned long timeCounter; ///< 时标
    unsigned int mslCnt;       ///< 导弹数量(<=MAX_MSL_NUM)
    /// \struct 导弹弹道信息
    struct MslTrajectoryInfo {
        unsigned int launchFlightID; ///< 发射机编号
        unsigned int mslStaIndex;    ///< 导弹挂点下标
        unsigned int mslLockedTgtID; ///< 导弹锁定目标编号
        double lon;                  ///< 导弹经度(rad)
        double lat;                  ///< 导弹纬度(rad)
        double alt;                  ///< 导弹海拔高度(m)
        double velNWU[3];            ///< 地理系速度矢量(m/s)
        double flyTime;              ///< 导弹已飞时间(s)
        double remainTime;           ///< 导弹剩余飞行时间(s)
        bool isSeekerCaptured;       ///< 导引头是否截获
        bool APoleValid;             ///< A极有效性
        bool FPoleValid;             ///< F极有效性
        double APoleTime;            ///< A极时间(s)
        double FPoleTime;            ///< F极时间(s)
        double APoleRange;           ///< A极距离(m)
        double FPoleRange;           ///< F极距离(m)
    } trajectoryInfos[MAX_MSL_NUM]; ///< 导弹弹道信息
};

//------------------------------------------

/// \struct 编队成员飞机状态数据
struct COFlightStatus
{
    unsigned int flightMemCnt; ///< 飞机成员数
    ACFlightStatus memFlightStatus[MAX_FLIGHT_NUM]; ///< 成员飞机状态数据信息
};

/// \struct 编队成员雷达探测目标
struct CORdrTarget
{
    unsigned int flightMemCnt; ///< 飞机成员数
    ACRdrTarget memRdrTarget[MAX_FLIGHT_NUM]; ///< 成员雷达探测目标信息
};

/// \struct 编队成员电子战探测目标
struct COEwsTarget
{
    unsigned int flightMemCnt; ///< 飞机成员数
    ACEwsTarget memEwsTarget[MAX_FLIGHT_NUM]; ///< 成员电子战探测目标信息
};

/// \struct 编队成员电子战导弹告警
struct COMslWarning
{
    unsigned int flightMemCnt; ///< 飞机成员数
    ACMslWarning memMslWarning[MAX_FLIGHT_NUM]; ///< 成员电子战导弹告警信息
};

/// \struct 编队成员发射火控包线
struct COFCCStatus
{
    unsigned int flightMemCnt; ///< 飞机成员数
    ACFCCStatus memFCCStatus[MAX_FLIGHT_NUM]; ///< 成员发射火控包线信息
};

/// \struct 编队成员制导武器信息
struct COMSLInGuide
{
    unsigned int flightMemCnt; ///< 飞机成员数
    ACMSLInGuide memMSLInGuide[MAX_FLIGHT_NUM]; ///< 成员制导武器信息信息
};

//------------------------------------------

/// \struct 飞行控制指令
struct FlyControlCmd
{
	unsigned int executePlaneID; ///< 目的飞机编号
    bool velCtrlCmd;        ///< 速度矢量指令
    bool navCtrlCmd;        ///< 航路飞行指令
    bool altCtrlCmd;        ///< 高度保持指令
    bool headCtrlCmd;       ///< 航向保持指令
    bool speedCtrlCmd;      ///< 速度保持指令
    double desireVelNWU[3]; ///< 期望速度矢量(m/s)
    double desireNavLon;    ///< 期望航路经度(rad)
    double desireNavLat;    ///< 期望航路纬度(rad)
    double desireNavAlt;    ///< 期望航路高度(m)
    double desireAlt;       ///< 期望高度(m)
    double desireHead;      ///< 期望航路航向(rad)
    double desireSpeed;     ///< 期望航路速度(m/s)
    unsigned int _cmdCnt;   ///< 指令计数
};

/// \struct 传感器控制指令
struct SnsControlCmd
{
    unsigned int executePlaneID; ///< 目的飞机编号
    FUNC_STATUS rdrOpenCmd;      ///< 雷达开关指令 \sa FUNC_STATUS
    FUNC_STATUS ewsOpenCmd;      ///< 电子战开关指令 \sa FUNC_STATUS
    FUNC_STATUS linkOpenCmd;     ///< 数据链开关指令 \sa FUNC_STATUS
    unsigned int _cmdCnt;        ///< 指令计数
};

/// \struct 武器控制指令
struct WpnControlCmd
{
    unsigned int launchPlaneID; ///< 发射机编号
    unsigned int guidePlaneID;  ///< 制导机编号
    unsigned int mslLockTgtID;  ///< 导弹锁定目标编号
    unsigned int _cmdCnt;       ///< 指令计数
};

//------------------------------------------

/// \struct 编队内部数据包
struct InTeamDataBag
{
    unsigned int DST_PLANE_ID;  ///< 接收飞机编号
    unsigned int DATA_BAG_TYPE; ///< 数据包类型
    char DATA[MAX_ITDB_DATA_LEN]; ///< 数据包
};

//------------------------------------------

/// \struct 飞机接口事件日志结构体
struct EventLog
{
    char EventDes[MAX_EVENT_LOG_STRING_LEN]; ///< 自由文电
};

//------------------------------------------

/// \typedef 回调函数_飞行控制
typedef bool (* P_CALL_BACK_FLY_CTRL) (const FlyControlCmd&);

/// \typedef 回调函数_传感器控制
typedef bool (* P_CALL_BACK_SNS_CTRL) (const SnsControlCmd&);

/// \typedef 回调函数_武器控制
typedef bool (* P_CALL_BACK_WPN_CTRL) (const WpnControlCmd&);

/// \typedef 回调函数_编队内部数据包发送
typedef bool (* P_CALL_BACK_SEND_ITDB) (const InTeamDataBag&);

/// \typedef 回调函数_事件日志记录
typedef bool (* P_CALL_BACK_EVENT_LOG) (const EventLog&);

//------------------------------------------
}

#endif // STRATEGY_IO_H
