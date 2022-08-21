#include "my_strategy.h"
#include "memory.h"

#define MY_STRATEGT_VERSION 0x010001
#define MY_STRATEGT_VERSION_STR "1.00.001"

CStrategy* CreateStrategyInstance()
{
    static MyStrategy *myStrategyInstance = new MyStrategy();
    return myStrategyInstance;
}

MyStrategy::MyStrategy()
{
    initData();
}

unsigned int MyStrategy::version() const { return MY_STRATEGT_VERSION; }
const char* MyStrategy::name() const { return "Your name"; }
const char* MyStrategy::slogan() const { return "Your slogan!"; }
const char* MyStrategy::info() const { return "'Your info."; }
const char* MyStrategy::organization() const { return "Your organization"; }
const char* MyStrategy::author() const { return "Author"; }
const char* MyStrategy::teamMembers() const {
    return " teamMembers£ºZhang San, Li Si";
}

void MyStrategy::initData()
{
    memset(&mPKConfig, 0, sizeof(mPKConfig));
    memset(&mACFlightStatus, 0, sizeof(mACFlightStatus));
    memset(&mACRdrTarget, 0, sizeof(mACRdrTarget));
    memset(&mACEwsTarget, 0, sizeof(mACEwsTarget));
    memset(&mACMslWarning, 0, sizeof(mACMslWarning));
    memset(&mACFCCStatus, 0, sizeof(mACFCCStatus));
    memset(&mACMSLInGuide, 0, sizeof(mACMSLInGuide));
    memset(&mTeamMSLLaunched, 0, sizeof(mTeamMSLLaunched));
    memset(&mCOFlightStatus, 0, sizeof(mCOFlightStatus));
    memset(&mCORdrTarget, 0, sizeof(mCORdrTarget));
    memset(&mCOEwsTarget, 0, sizeof(mCOEwsTarget));
    memset(&mCOMslWarning, 0, sizeof(mCOMslWarning));
    memset(&mCOFCCStatus, 0, sizeof(mCOFCCStatus));
    memset(&mCOMSLInGuide, 0, sizeof(mCOMSLInGuide));

	int m_lastWpnShootTimeCounter = 0;
}
