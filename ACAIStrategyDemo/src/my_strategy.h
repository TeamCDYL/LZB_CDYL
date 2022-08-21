#ifndef MY_STRATEGY_H
#define MY_STRATEGY_H

#include "strategy_interface.h"
#include <Python.h>
#include "strategy_interface.h"
#include "file_watcher.h"
#include <math.h>

extern "C"{
	__declspec(dllexport) int ffunc(int a, int b);
}

extern int g_flight_state;	//飞机存活状态
extern int g_cnt_state;		//导弹威胁状态
extern int g_enmy_state;	//敌机数量状态
extern int g_launch_state;	//我方发射导弹状态
extern int g_guide_state;	//我方制导导弹状态

/// \brief 策略实现Demo
class MyStrategy : public CStrategy
{
public:

	//攻击动作集
	void attack_i();

	/// \brief 默认构造函数
	MyStrategy();

	/// \brief 获取策略版本号
	/// \return 策略版本号
	unsigned int version() const;

	/// \brief 获取策略名称
	/// \return 策略名称
	const char* name() const;

	/// \brief 获取策略标语
	/// \return 策略标语
	const char* slogan() const;

	/// \brief 获取策略简述
	/// \return 策略简述
	const char* info() const;

	/// \brief 获取开发组织/机构
	/// \return 开发组织/机构
	const char* organization() const;

	/// \brief 获取开发作者/团队作者代表
	/// \return 开发作者或开发团队作者代表
	const char* author() const;

	/// \brief 获取开发作者团队成员名单
	/// \return 开发作者团队成员名单
	const char* teamMembers() const;

public:
	//机动动作集调用
	void maneuver_i(int fin,int sin );

    /// \brief 比赛开始处理函数
    /// \details 每局比赛开始时调用
    /// \param[in] pkConfig 比赛配置信息 \sa ACAI::PKConfig
    void onPKStart(const ACAI::PKConfig &pkConfig);

	/// \brief 比赛结束处理函数
	/// \details 每局比赛结束时调用
	/// \param[in] pkResult 比赛结果信息 \sa ACAI::pkResult
	void onPKEnd();

	/// \brief 比赛飞机出局事件函数
	/// \details 友方或敌方飞机被命中或出界退出房间时调用
	/// \param[in] flightID 飞机编号 
	void onPKOut(unsigned int flightID);

	/// \brief 本机飞机状态数据数据更新
	/// \param[in] inputData 飞机状态数据 \sa ACAI::ACFlightStatus
	void onACFlightStatusUpdate(const ACAI::ACFlightStatus &inputData);

	/// \brief 本机雷达探测目标数据更新
	/// \param[in] inputData 雷达探测目标 \sa ACAI::ACRdrTarget
	void onACRdrTargetUpdate(const ACAI::ACRdrTarget &inputData);

	/// \brief 本机电子战探测目标数据更新
	/// \param[in] inputData 电子战探测目标 \sa ACAI::ACEwsTarget
	void onACEwsTargetUpdate(const ACAI::ACEwsTarget &inputData);

	/// \brief 本机导弹威胁告警数据更新
	/// \param[in] inputData 导弹威胁告警 \sa ACAI::ACMslWarning
	void onACMslWarningUpdate(const ACAI::ACMslWarning &inputData);

	/// \brief 本机发射火控包线数据更新
	/// \param[in] inputData 发射火控包线 \sa ACAI::ACFCCStatus
	void onACFCCStatusUpdate(const ACAI::ACFCCStatus &inputData);

	/// \brief 本机制导武器信息数据更新
	/// \param[in] inputData 制导武器信息 \sa ACAI::ACMSLInGuide
	void onACMSLInGuideUpdate(const ACAI::ACMSLInGuide &inputData);

	/// \brief 编队发射导弹实时信息数据更新
	/// \param[in] inputData 编队发射导弹实时信息 \sa ACAI::TeamMSLLaunched
	void onTeamMSLLaunchedUpdate(const ACAI::TeamMSLLaunched &inputData);

	/// \brief 编队成员飞机状态数据数据更新
	/// \param[in] inputData 编队成员飞机状态数据 \sa ACAI::COFlightStatus
	void onCOFlightStatusUpdate(const ACAI::COFlightStatus &inputData);

	/// \brief 编队成员雷达探测目标数据更新
	/// \param[in] inputData 编队成员雷达探测目标 \sa ACAI::CORdrTarget
	void onCORdrTargetUpdate(const ACAI::CORdrTarget &inputData);

	/// \brief 编队成员电子战探测目标数据更新
	/// \param[in] inputData 编队成员电子战探测目标 \sa ACAI::COEwsTarget
	void onCOEwsTargetUpdate(const ACAI::COEwsTarget &inputData);

	/// \brief 编队成员电子战导弹告警数据更新
	/// \param[in] inputData 编队成员电子战导弹告警 \sa ACAI::COMslWarning
	void onCOMslWarningUpdate(const ACAI::COMslWarning &inputData);

	/// \brief 编队成员发射火控包线数据更新
	/// \param[in] inputData 编队成员发射火控包线 \sa ACAI::COFCCStatus
	void onCOFCCStatusUpdate(const ACAI::COFCCStatus &inputData);

	/// \brief 编队成员制导武器信息数据更新
	/// \param[in] inputData 编队成员制导武器信息 \sa ACAI::COMSLInGuide
	void onCOMSLInGuideUpdate(const ACAI::COMSLInGuide &inputData);

	/// \brief 编队成员编队内部数据包更新
	/// \param[in] inputData 编队成员编队内部数据包 \sa ACAI::InTeamDataBag
	void onCOTeamDataBagUpdate(const ACAI::InTeamDataBag &inputData);

	///---------------------------------------

	/// \brief 后台任务
	/// \details 后台调用线程
	void backGround();

	/// \brief 20ms周期任务
	/// \details 20ms周期线程
	void timeSlice20();

	/// \brief 40ms周期任务
	/// \details 40ms周期线程
	void timeSlice40();

private:
	/// \brief 初始化数据
	void initData();

private:
	ACAI::PKConfig mPKConfig;               ///< 比赛配置信息 \sa ACAI::PKConfig
	ACAI::ACFlightStatus mACFlightStatus;   ///< 本机飞机状态数据 \sa ACAI::ACFlightStatus
	ACAI::ACRdrTarget mACRdrTarget;         ///< 本机雷达探测目标 \sa ACAI::ACRdrTarget
	ACAI::ACEwsTarget mACEwsTarget;         ///< 本机电子战探测目标 \sa ACAI::ACEwsTarget
	ACAI::ACMslWarning mACMslWarning;       ///< 本机导弹威胁告警 \sa ACAI::ACMslWarning
	ACAI::ACFCCStatus mACFCCStatus;         ///< 本机发射火控包线 \sa ACAI::ACFCCStatus
	ACAI::ACMSLInGuide mACMSLInGuide;       ///< 本机制导武器信息 \sa ACAI::ACMSLInGuide
	ACAI::TeamMSLLaunched mTeamMSLLaunched; ///< 编队发射导弹实时信息 \sa ACAI::TeamMSLLaunched
	ACAI::COFlightStatus mCOFlightStatus;   ///< 编队成员飞机状态数据 \sa ACAI::COFlightStatus
	ACAI::CORdrTarget mCORdrTarget;         ///< 编队成员雷达探测目标 \sa ACAI::CORdrTarget
	ACAI::COEwsTarget mCOEwsTarget;         ///< 编队成员电子战探测目标 \sa ACAI::COEwsTarget
	ACAI::COMslWarning mCOMslWarning;       ///< 编队成员电子战导弹告警 \sa ACAI::COMslWarning
	ACAI::COFCCStatus mCOFCCStatus;         ///< 编队成员发射火控包线 \sa ACAI::COFCCStatus
	ACAI::COMSLInGuide mCOMSLInGuide;       ///< 编队成员制导武器信息 \sa ACAI::COMSLInGuide
	ACAI::InTeamDataBag mCOTeamDataBag;     ///< 编队成员编队内部数据包 \sa ACAI::InTeamDataBag

private:
	//向敌方防线飞行
	void DoTacPointAtk();
	//向一架敌机飞行
	void DoTacToTar();
	//+30度加速爬升
	void DoTacAltClimbP30();
	//+60度加速爬升
	void DoTacAltClimbP60();
	//-30度下潜
	void DoTacNoseDiveM30();
	//-60度下潜
	void DoTacNoseDiveM60();
	//蛇形机动
	void DoTacStaHov();
	//左转偏置30
	void DoTurnLeft30();
	//左转偏置60
	void DoTurnLeft60();
	//右转偏置30
	void DoTurnRight30();
	//右转偏置60
	void DoTurnRight60();
	//掉头
	void DoTacHeadEvade();
	//回环
	void DoTacCir();
	//掉头后30度下潜
	void DoTurnEvad30();
	//掉头后60度下潜
	void DoTurnEvad60();



	//左转向
	void DoTurnLeft();
	//右转向
	void DoTurnRight();
	//向前飞行
	void DoTurnFor();

	//攻击动作集
	//武器发射
	void DoTacWpnShoot(int m = 0);
	//切换制导机
	void SwitchGuideFlight();

	void DoTacHeadGuide();
	// 动作读取
	void readAction();

	// 输出状态
	void PrintStatus(const char * filename);

	//
	void SetFlightState(unsigned int flightID);
	//奖赏函数
	int OutputReward();
	//打印出奖赏值
	void PrintReward();


};

#endif // MY_STRATEGY_H
