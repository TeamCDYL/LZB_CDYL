/*******************************************************************************
 * \file        strategy_interface.cpp
 * \authors     weiqm<4146>, linwx<4204>
 * \copyright   Copyright (c) 2021, 611.CADI All rights reserved.
 *******************************************************************************/

#include "strategy_interface.h"

void CStrategy::registerFlyCtrlCallback(ACAI::P_CALL_BACK_FLY_CTRL pCallBack)
{
    printf("[CStrategy] CALL_BACK_FLY_CTRL registered at %08x.\n", pCallBack);
    m_pFlyCtrl = pCallBack;
}

void CStrategy::registerSnsCtrlCallback(ACAI::P_CALL_BACK_SNS_CTRL pCallBack)
{
    printf("[CStrategy] CALL_BACK_SNS_CTRL registered at %08x.\n", pCallBack);
    m_pSnsCtrl = pCallBack;
}

void CStrategy::registerWpnCtrlCallback(ACAI::P_CALL_BACK_WPN_CTRL pCallBack)
{
    printf("[CStrategy] CALL_BACK_WPN_CTRL registered at %08x.\n", pCallBack);
    m_pWpnCtrl = pCallBack;
}

void CStrategy::registerSendITDBCallback(ACAI::P_CALL_BACK_SEND_ITDB pCallBack)
{
    printf("[CStrategy] CALL_BACK_SEND_ITDB registered at %08x.\n", pCallBack);
    m_pSendITDB = pCallBack;
}

void CStrategy::registereventLogCallback(ACAI::P_CALL_BACK_EVENT_LOG pCallBack)
{
    printf("[CStrategy] CALL_BACK_EVENT_LOG registered at %08x.\n", pCallBack);
    m_pEventLog = pCallBack;
}


void CStrategy::sendFlyControlCmd(const ACAI::FlyControlCmd &outputData) const
{
    if (!m_pFlyCtrl || !m_pFlyCtrl(outputData)) {
        if (!m_pFlyCtrl) {
            printf("[CStrategy] 飞行控制指令发送接口未注册！\n");
        }
        else {
            printf("[CStrategy] 飞行控制指令发送失败！\n");
        }
    }
}

void CStrategy::sendSnsControlCmd(const ACAI::SnsControlCmd &outputData) const
{
    if (!m_pSnsCtrl || !m_pSnsCtrl(outputData)) {
        if (!m_pSnsCtrl) {
            printf("[CStrategy] 传感器控制指令发送接口未注册！\n");
        }
        else {
            printf("[CStrategy] 传感器控制指令发送失败！\n");
        }
    }
}

void CStrategy::sendWpnControlCmd(const ACAI::WpnControlCmd &outputData) const
{
    if (!m_pWpnCtrl || !m_pWpnCtrl(outputData)) {
        if (!m_pWpnCtrl) {
            printf("[CStrategy] 武器控制指令发送接口未注册！\n");
        }
        else {
            printf("[CStrategy] 武器控制指令发送失败！\n");
        }
    }
}

void CStrategy::sendInTeamDataBag(const ACAI::InTeamDataBag &outputData) const
{
    if (!m_pSendITDB || !m_pSendITDB(outputData)) {
        if (!m_pSendITDB) {
            printf("[CStrategy] 编队内部数据包发送接口未注册！\n");
        }
        else {
            printf("[CStrategy] 编队内部数据包发送失败！\n");
        }
    }
}

void CStrategy::logEvent(const ACAI::EventLog &outputData) const
{
    if (!m_pEventLog || !m_pEventLog(outputData)) {
        if (!m_pEventLog) {
            printf("[CStrategy] 事件日志记录接口未注册！\n");
        }
        else {
            printf("[CStrategy] 事件日志记录失败！\n");
        }
    }
}
