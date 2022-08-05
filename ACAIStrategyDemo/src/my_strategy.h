#ifndef MY_STRATEGY_H
#define MY_STRATEGY_H

#include "strategy_interface.h"

extern "C"{
	__declspec(dllexport) int ffunc(int a, int b);
}

/// \brief ����ʵ��Demo
class MyStrategy : public CStrategy
{
public:
    /// \brief Ĭ�Ϲ��캯��
    MyStrategy();

    /// \brief ��ȡ���԰汾��
    /// \return ���԰汾��
    unsigned int version() const;

    /// \brief ��ȡ��������
    /// \return ��������
    const char* name() const;

    /// \brief ��ȡ���Ա���
    /// \return ���Ա���
    const char* slogan() const;

    /// \brief ��ȡ���Լ���
    /// \return ���Լ���
    const char* info() const;

    /// \brief ��ȡ������֯/����
    /// \return ������֯/����
    const char* organization() const;

    /// \brief ��ȡ��������/�Ŷ����ߴ���
    /// \return �������߻򿪷��Ŷ����ߴ���
    const char* author() const;

    /// \brief ��ȡ���������Ŷӳ�Ա����
    /// \return ���������Ŷӳ�Ա����
    const char* teamMembers() const;

public:
    /// \brief ������ʼ������
    /// \details ÿ�ֱ�����ʼʱ����
    /// \param[in] pkConfig ����������Ϣ \sa ACAI::PKConfig
    void onPKStart(const ACAI::PKConfig &pkConfig);

    /// \brief ��������������
    /// \details ÿ�ֱ�������ʱ����
    /// \param[in] pkResult ���������Ϣ \sa ACAI::pkResult
    void onPKEnd();

	/// \brief �����ɻ������¼�����
    /// \details �ѷ���з��ɻ������л�����˳�����ʱ����
    /// \param[in] flightID �ɻ���� 
    void onPKOut(unsigned int flightID);

    /// \brief �����ɻ�״̬�������ݸ���
    /// \param[in] inputData �ɻ�״̬���� \sa ACAI::ACFlightStatus
    void onACFlightStatusUpdate(const ACAI::ACFlightStatus &inputData);

    /// \brief �����״�̽��Ŀ�����ݸ���
    /// \param[in] inputData �״�̽��Ŀ�� \sa ACAI::ACRdrTarget
    void onACRdrTargetUpdate(const ACAI::ACRdrTarget &inputData);

    /// \brief ��������ս̽��Ŀ�����ݸ���
    /// \param[in] inputData ����ս̽��Ŀ�� \sa ACAI::ACEwsTarget
    void onACEwsTargetUpdate(const ACAI::ACEwsTarget &inputData);

    /// \brief ����������в�澯���ݸ���
    /// \param[in] inputData ������в�澯 \sa ACAI::ACMslWarning
    void onACMslWarningUpdate(const ACAI::ACMslWarning &inputData);

    /// \brief ���������ذ������ݸ���
    /// \param[in] inputData �����ذ��� \sa ACAI::ACFCCStatus
    void onACFCCStatusUpdate(const ACAI::ACFCCStatus &inputData);

    /// \brief �����Ƶ�������Ϣ���ݸ���
    /// \param[in] inputData �Ƶ�������Ϣ \sa ACAI::ACMSLInGuide
    void onACMSLInGuideUpdate(const ACAI::ACMSLInGuide &inputData);

    /// \brief ��ӷ��䵼��ʵʱ��Ϣ���ݸ���
    /// \param[in] inputData ��ӷ��䵼��ʵʱ��Ϣ \sa ACAI::TeamMSLLaunched
    void onTeamMSLLaunchedUpdate(const ACAI::TeamMSLLaunched &inputData);

    /// \brief ��ӳ�Ա�ɻ�״̬�������ݸ���
    /// \param[in] inputData ��ӳ�Ա�ɻ�״̬���� \sa ACAI::COFlightStatus
    void onCOFlightStatusUpdate(const ACAI::COFlightStatus &inputData);

    /// \brief ��ӳ�Ա�״�̽��Ŀ�����ݸ���
    /// \param[in] inputData ��ӳ�Ա�״�̽��Ŀ�� \sa ACAI::CORdrTarget
    void onCORdrTargetUpdate(const ACAI::CORdrTarget &inputData);

    /// \brief ��ӳ�Ա����ս̽��Ŀ�����ݸ���
    /// \param[in] inputData ��ӳ�Ա����ս̽��Ŀ�� \sa ACAI::COEwsTarget
    void onCOEwsTargetUpdate(const ACAI::COEwsTarget &inputData);

    /// \brief ��ӳ�Ա����ս�����澯���ݸ���
    /// \param[in] inputData ��ӳ�Ա����ս�����澯 \sa ACAI::COMslWarning
    void onCOMslWarningUpdate(const ACAI::COMslWarning &inputData);

    /// \brief ��ӳ�Ա�����ذ������ݸ���
    /// \param[in] inputData ��ӳ�Ա�����ذ��� \sa ACAI::COFCCStatus
    void onCOFCCStatusUpdate(const ACAI::COFCCStatus &inputData);

    /// \brief ��ӳ�Ա�Ƶ�������Ϣ���ݸ���
    /// \param[in] inputData ��ӳ�Ա�Ƶ�������Ϣ \sa ACAI::COMSLInGuide
    void onCOMSLInGuideUpdate(const ACAI::COMSLInGuide &inputData);

	/// \brief ��ӳ�Ա����ڲ����ݰ�����
    /// \param[in] inputData ��ӳ�Ա����ڲ����ݰ� \sa ACAI::InTeamDataBag
    void onCOTeamDataBagUpdate(const ACAI::InTeamDataBag &inputData);

    ///---------------------------------------

    /// \brief ��̨����
    /// \details ��̨�����߳�
    void backGround();

    /// \brief 20ms��������
    /// \details 20ms�����߳�
    void timeSlice20();

    /// \brief 40ms��������
    /// \details 40ms�����߳�
    void timeSlice40();

private:
    /// \brief ��ʼ������
    void initData();

private:
    ACAI::PKConfig mPKConfig;               ///< ����������Ϣ \sa ACAI::PKConfig
    ACAI::ACFlightStatus mACFlightStatus;   ///< �����ɻ�״̬���� \sa ACAI::ACFlightStatus
    ACAI::ACRdrTarget mACRdrTarget;         ///< �����״�̽��Ŀ�� \sa ACAI::ACRdrTarget
    ACAI::ACEwsTarget mACEwsTarget;         ///< ��������ս̽��Ŀ�� \sa ACAI::ACEwsTarget
    ACAI::ACMslWarning mACMslWarning;       ///< ����������в�澯 \sa ACAI::ACMslWarning
    ACAI::ACFCCStatus mACFCCStatus;         ///< ���������ذ��� \sa ACAI::ACFCCStatus
    ACAI::ACMSLInGuide mACMSLInGuide;       ///< �����Ƶ�������Ϣ \sa ACAI::ACMSLInGuide
    ACAI::TeamMSLLaunched mTeamMSLLaunched; ///< ��ӷ��䵼��ʵʱ��Ϣ \sa ACAI::TeamMSLLaunched
    ACAI::COFlightStatus mCOFlightStatus;   ///< ��ӳ�Ա�ɻ�״̬���� \sa ACAI::COFlightStatus
    ACAI::CORdrTarget mCORdrTarget;         ///< ��ӳ�Ա�״�̽��Ŀ�� \sa ACAI::CORdrTarget
    ACAI::COEwsTarget mCOEwsTarget;         ///< ��ӳ�Ա����ս̽��Ŀ�� \sa ACAI::COEwsTarget
    ACAI::COMslWarning mCOMslWarning;       ///< ��ӳ�Ա����ս�����澯 \sa ACAI::COMslWarning
    ACAI::COFCCStatus mCOFCCStatus;         ///< ��ӳ�Ա�����ذ��� \sa ACAI::COFCCStatus
    ACAI::COMSLInGuide mCOMSLInGuide;       ///< ��ӳ�Ա�Ƶ�������Ϣ \sa ACAI::COMSLInGuide
	ACAI::InTeamDataBag mCOTeamDataBag;     ///< ��ӳ�Ա����ڲ����ݰ� \sa ACAI::InTeamDataBag

private:
	// ����ͻ��
	void DoTacPointAtk();
	// ��β����
	void DoTacHeadEvade();
	// ƫ���Ƶ�
	void DoTacHeadGuide();
	// ��������
	void DoTacAltClimb();
	// ��������
	unsigned long m_lastWpnShootTimeCounter; //�ϴη���ʱ��
	void DoTacWpnShoot();
};

#endif // MY_STRATEGY_H
