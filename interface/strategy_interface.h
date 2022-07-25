/*******************************************************************************
 * \file        strategy_interface.h
 * \authors     weiqm<4146>, linwx<4204>
 * \copyright   Copyright (c) 2021, 611.CADI All rights reserved.
 *******************************************************************************/

#ifndef STRATEGY_INTERFACE_H
#define STRATEGY_INTERFACE_H

#include "strategy_global.h"
#include "strategy_io.h"
#include <stdio.h>

// STRATEGT_INTF_VERSION is (major << 16) + (minor << 8) + patch.
#define STRATEGT_INTF_VERSION 0x020000
#define STRATEGT_INTF_VERSION_STR "2.01"

#ifndef UNUSED
#define UNUSED(para) (void)para;
#endif

/// \brief AI空战策略模块接口
class STRATEGY_EXPORT CStrategy
{
public:
    /// \brief 默认构造函数
    CStrategy()
        : m_pFlyCtrl(0), m_pSnsCtrl(0), m_pWpnCtrl(0)
        , m_pSendITDB(0), m_pEventLog(0)
    {}

    /// \brief 默认虚析构函数
    virtual ~CStrategy() {}

    /// \brief 获取策略版本号
    /// \return 策略版本号
    virtual unsigned int version() const { return 0xFFFFFF; }

    /// \brief 获取策略名称
    /// \return 策略名称
    virtual const char* name() const { return "<AI_NAME>"; }

    /// \brief 获取策略标语
    /// \return 策略标语
    virtual const char* slogan() const { return "<AI_SLOGAN>"; }

    /// \brief 获取策略简述
    /// \return 策略简述
    virtual const char* info() const { return "<AI_INFO>"; }

    /// \brief 获取开发组织/机构
    /// \return 开发组织/机构
    virtual const char* organization() const { return "<AI_ORGANIZATION>"; }

    /// \brief 获取开发作者/团队作者代表
    /// \return 开发作者或开发团队作者代表
    virtual const char* author() const { return "<AI_AUTHOR>"; }

    /// \brief 获取开发作者团队成员名单
    /// \return 开发作者团队成员名单
    virtual const char* teamMembers() const { return "<AI_TEAM_MEMBERS>"; }

    /// \brief 比赛开始处理函数
    /// \details 每局比赛开始时调用
    /// \param[in] pkConfig 比赛配置信息 \sa ACAI::PKConfig
    virtual void onPKStart(const ACAI::PKConfig &pkConfig) { UNUSED(pkConfig); }

    /// \brief 比赛结束处理函数
    /// \details 每局比赛结束时调用
    virtual void onPKEnd() { }

	/// \brief 比赛飞机出局事件函数
    /// \details 友方或敌方飞机被命中或出界退出房间时调用
    /// \param[in] flightID 飞机编号 
    virtual void onPKOut(unsigned int flightID) { UNUSED(flightID); }

    ///---------------------------------------

    /// \brief 本机飞机状态数据数据更新
    /// \param[in] inputData 飞机状态数据 \sa ACAI::ACFlightStatus
    virtual void onACFlightStatusUpdate(const ACAI::ACFlightStatus &inputData) { UNUSED(inputData); }

    /// \brief 本机雷达探测目标数据更新
    /// \param[in] inputData 雷达探测目标 \sa ACAI::ACRdrTarget
    virtual void onACRdrTargetUpdate(const ACAI::ACRdrTarget &inputData) { UNUSED(inputData); }

    /// \brief 本机电子战探测目标数据更新
    /// \param[in] inputData 电子战探测目标 \sa ACAI::ACEwsTarget
    virtual void onACEwsTargetUpdate(const ACAI::ACEwsTarget &inputData) { UNUSED(inputData); }

    /// \brief 本机导弹威胁告警数据更新
    /// \param[in] inputData 导弹威胁告警 \sa ACAI::ACMslWarning
    virtual void onACMslWarningUpdate(const ACAI::ACMslWarning &inputData) { UNUSED(inputData); }

    /// \brief 本机发射火控包线数据更新
    /// \param[in] inputData 发射火控包线 \sa ACAI::ACFCCStatus
    virtual void onACFCCStatusUpdate(const ACAI::ACFCCStatus &inputData) { UNUSED(inputData); }

    /// \brief 本机制导武器信息数据更新
    /// \param[in] inputData 制导武器信息 \sa ACAI::ACMSLInGuide
    virtual void onACMSLInGuideUpdate(const ACAI::ACMSLInGuide &inputData) { UNUSED(inputData); }

    /// \brief 编队发射导弹实时信息数据更新
    /// \param[in] inputData 编队发射导弹实时信息 \sa ACAI::TeamMSLLaunched
    virtual void onTeamMSLLaunchedUpdate(const ACAI::TeamMSLLaunched &inputData) { UNUSED(inputData); }

    /// \brief 编队成员飞机状态数据数据更新
    /// \param[in] inputData 编队成员飞机状态数据 \sa ACAI::COFlightStatus
    virtual void onCOFlightStatusUpdate(const ACAI::COFlightStatus &inputData) { UNUSED(inputData); }

    /// \brief 编队成员雷达探测目标数据更新
    /// \param[in] inputData 编队成员雷达探测目标 \sa ACAI::CORdrTarget
    virtual void onCORdrTargetUpdate(const ACAI::CORdrTarget &inputData) { UNUSED(inputData); }

    /// \brief 编队成员电子战探测目标数据更新
    /// \param[in] inputData 编队成员电子战探测目标 \sa ACAI::COEwsTarget
    virtual void onCOEwsTargetUpdate(const ACAI::COEwsTarget &inputData) { UNUSED(inputData); }

    /// \brief 编队成员电子战导弹告警数据更新
    /// \param[in] inputData 编队成员电子战导弹告警 \sa ACAI::COMslWarning
    virtual void onCOMslWarningUpdate(const ACAI::COMslWarning &inputData) { UNUSED(inputData); }

    /// \brief 编队成员发射火控包线数据更新
    /// \param[in] inputData 编队成员发射火控包线 \sa ACAI::COFCCStatus
    virtual void onCOFCCStatusUpdate(const ACAI::COFCCStatus &inputData) { UNUSED(inputData); }

    /// \brief 编队成员制导武器信息数据更新
    /// \param[in] inputData 编队成员制导武器信息 \sa ACAI::COMSLInGuide
    virtual void onCOMSLInGuideUpdate(const ACAI::COMSLInGuide &inputData) { UNUSED(inputData); }

	/// \brief 编队成员编队内部数据包更新
    /// \param[in] inputData 编队成员编队内部数据包 \sa ACAI::InTeamDataBag
    virtual void onCOTeamDataBagUpdate(const ACAI::InTeamDataBag &inputData) { UNUSED(inputData); }

    ///---------------------------------------

    /// \brief 后台任务
    /// \details 后台调用线程
    virtual void backGround() {}

    /// \brief 20ms周期任务
    /// \details 20ms周期线程
    virtual void timeSlice20() {}

    /// \brief 40ms周期任务
    /// \details 40ms周期线程
    virtual void timeSlice40() {}

public:
    /// \brief 注册飞行控制回调函数
    /// \param[in] pCallBack 飞行控制回调函数 \sa ACAI::P_CALL_BACK_FLY_CTRL
    void registerFlyCtrlCallback(ACAI::P_CALL_BACK_FLY_CTRL pCallBack);

    /// \brief 注册传感器控制回调函数
    /// \param[in] pCallBack 传感器控制回调函数 \sa ACAI::P_CALL_BACK_SNS_CTRL
    void registerSnsCtrlCallback(ACAI::P_CALL_BACK_SNS_CTRL pCallBack);

    /// \brief 注册武器控制回调函数
    /// \param[in] pCallBack 武器控制回调函数 \sa ACAI::P_CALL_BACK_WPN_CTRL
    void registerWpnCtrlCallback(ACAI::P_CALL_BACK_WPN_CTRL pCallBack);

    /// \brief 注册编队间消息发送回调函数
    /// \param[in] pCallBack 编队间消息发送回调函数 \sa ACAI::P_CALL_BACK_SEND_ITDB
    void registerSendITDBCallback(ACAI::P_CALL_BACK_SEND_ITDB pCallBack);

    /// \brief 注册事件日志记录回调函数
    /// \param[in] pCallBack 事件日志记录回调函数 \sa ACAI::P_CALL_BACK_EVENT_LOG
    void registereventLogCallback(ACAI::P_CALL_BACK_EVENT_LOG pCallBack);

protected:
    /// \brief 发送飞行控制指令
    /// \param[in] outputData 飞行控制指令 \sa ACAI::FlyControlCmd
    void sendFlyControlCmd(const ACAI::FlyControlCmd &outputData) const;

    /// \brief 发送传感器控制指令
    /// \param[in] outputData 传感器控制指令 \sa ACAI::SnsControlCmd
    void sendSnsControlCmd(const ACAI::SnsControlCmd &outputData) const;

    /// \brief 发送武器控制指令
    /// \param[in] outputData 武器控制指令 \sa ACAI::WpnControlCmd
    void sendWpnControlCmd(const ACAI::WpnControlCmd &outputData) const;

    /// \brief 发送编队内部数据包
    /// \param[in] outputData 编队内部数据包 \sa ACAI::InTeamDataBag
    void sendInTeamDataBag(const ACAI::InTeamDataBag &outputData) const;

    /// \brief 记录事件日志
    /// \param[in] outputData 事件日志 \sa ACAI::EventLog
    void logEvent(const ACAI::EventLog &outputData) const;

private:
    ACAI::P_CALL_BACK_FLY_CTRL m_pFlyCtrl;   ///< 指令控制回调函数 \sa ACAI::P_CALL_BACK_FLY_CTRL
    ACAI::P_CALL_BACK_SNS_CTRL m_pSnsCtrl;   ///< 提示消息输出回调函数 \sa ACAI::P_CALL_BACK_SNS_CTRL
    ACAI::P_CALL_BACK_WPN_CTRL m_pWpnCtrl;   ///< 指令控制回调函数 \sa ACAI::P_CALL_BACK_WPN_CTRL
    ACAI::P_CALL_BACK_SEND_ITDB m_pSendITDB; ///< 提示消息输出回调函数 \sa ACAI::P_CALL_BACK_SEND_ITDB
    ACAI::P_CALL_BACK_EVENT_LOG m_pEventLog; ///< 指令控制回调函数 \sa ACAI::P_CALL_BACK_EVENT_LOG
};

extern "C" {
    /// \brief 创建策略模块动态链接库实例
    /// \return 策略模块动态链接对象示例，创建失败返回0 \sa CStrategy
    STRATEGY_EXPORT CStrategy* CreateStrategyInstance();
}

#endif // STRATEGY_INTERFACE_H
