#include "my_strategy.h"
#include "memory.h"

void MyStrategy::onACFlightStatusUpdate(const ACAI::ACFlightStatus &inputData)
{
    memcpy(&mACFlightStatus, &inputData, sizeof(mACFlightStatus));
}

void MyStrategy::onACRdrTargetUpdate(const ACAI::ACRdrTarget &inputData)
{
    memcpy(&mACRdrTarget, &inputData, sizeof(mACRdrTarget));
}

void MyStrategy::onACEwsTargetUpdate(const ACAI::ACEwsTarget &inputData)
{
    memcpy(&mACEwsTarget, &inputData, sizeof(mACEwsTarget));
}

void MyStrategy::onACMslWarningUpdate(const ACAI::ACMslWarning &inputData)
{
    memcpy(&mACMslWarning, &inputData, sizeof(mACMslWarning));
}

void MyStrategy::onACFCCStatusUpdate(const ACAI::ACFCCStatus &inputData)
{
    memcpy(&mACFCCStatus, &inputData, sizeof(mACFCCStatus));
}

void MyStrategy::onACMSLInGuideUpdate(const ACAI::ACMSLInGuide &inputData)
{
    memcpy(&mACMSLInGuide, &inputData, sizeof(mACMSLInGuide));
}

void MyStrategy::onTeamMSLLaunchedUpdate(const ACAI::TeamMSLLaunched &inputData)
{
    memcpy(&mTeamMSLLaunched, &inputData, sizeof(mTeamMSLLaunched));
}

void MyStrategy::onCOFlightStatusUpdate(const ACAI::COFlightStatus &inputData)
{
    memcpy(&mCOFlightStatus, &inputData, sizeof(mCOFlightStatus));
}

void MyStrategy::onCORdrTargetUpdate(const ACAI::CORdrTarget &inputData)
{
    memcpy(&mCORdrTarget, &inputData, sizeof(mCORdrTarget));
}

void MyStrategy::onCOEwsTargetUpdate(const ACAI::COEwsTarget &inputData)
{
    memcpy(&mCOEwsTarget, &inputData, sizeof(mCOEwsTarget));
}

void MyStrategy::onCOMslWarningUpdate(const ACAI::COMslWarning &inputData)
{
    memcpy(&mCOMslWarning, &inputData, sizeof(mCOMslWarning));
}

void MyStrategy::onCOFCCStatusUpdate(const ACAI::COFCCStatus &inputData)
{
    memcpy(&mCOFCCStatus, &inputData, sizeof(mCOFCCStatus));
}

void MyStrategy::onCOMSLInGuideUpdate(const ACAI::COMSLInGuide &inputData)
{
    memcpy(&mCOMSLInGuide, &inputData, sizeof(mCOMSLInGuide));
}

void MyStrategy::onCOTeamDataBagUpdate(const ACAI::InTeamDataBag &inputData)
{
	memcpy(&mCOTeamDataBag, &inputData, sizeof(mCOTeamDataBag));
}
