#include "my_strategy.h"
#include "memory.h"

#define MY_STRATEGT_VERSION 0x020002
#define MY_STRATEGT_VERSION_STR "2.00.002"

CStrategy* CreateStrategyInstance()
{
    static MyStrategy *myStrategyInstance = new MyStrategy();
    return myStrategyInstance;
}

MyStrategy::MyStrategy()
{
    initData(1);
}

unsigned int MyStrategy::version() const { return MY_STRATEGT_VERSION; }
const char* MyStrategy::name() const { return "Your name"; }
const char* MyStrategy::slogan() const { return "Your slogan!"; }
const char* MyStrategy::info() const { return "'Your info."; }
const char* MyStrategy::organization() const { return "Your organization"; }
const char* MyStrategy::author() const { return "Author"; }
const char* MyStrategy::teamMembers() const {
    return " teamMembers：Zhang San, Li Si";
}

void MyStrategy::initData(int role)
{
	action_finished = true;
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
	memset(&mLog, 0, sizeof(mLog));
	memset(&mCmd, 0, sizeof(mCmd));

	HeightEdge = (mPKConfig.MaxFlyHeight - mPKConfig.MinFlyHeight) / 20.0;
	LonEdge = (mPKConfig.RightUpLon - mPKConfig.LeftDownLon) / 20.0;
	LatEdge = (mPKConfig.RightUpLat - mPKConfig.LeftDownLat) / 20.0;
	HeightCorrect = (mPKConfig.MaxFlyHeight - mPKConfig.MinFlyHeight) / 20.0;
	LonCorrect = (mPKConfig.RightUpLon - mPKConfig.LeftDownLon) / 20.0;
	LatCorrect = (mPKConfig.RightUpLat - mPKConfig.LeftDownLat) / 20.0;

	dis2Lons = 160000 / (mPKConfig.RightUpLon - mPKConfig.LeftDownLon);
	dis2Lats = 80000 / (mPKConfig.RightUpLat - mPKConfig.LeftDownLat);

	flightRole = role;

	m_fallen_num = 0;
	t_fallen_num = 0;

	train_config.shoot_down = 150;	// 擊落
	train_config.win = 300;			// 游戲勝利
	train_config.in_warning = 6;	// 被導彈鎖定
	train_config.out_warning = 18;	// 逃脫導彈鎖定
	train_config.get_target = 10;	// 獲取敵方視野
	train_config.lost_wpn = 30;
	train_config.dis_adv = 5;		// 距離優勢
	train_config.alt_adv = 5;		// 高度優勢
	train_config.ang_adv = 5;		// 角度優勢
	train_config.win_dav = 5;		// 距終點距離
	train_config.tgt_num = 2;

	m_last_shoot_time = mACFlightStatus.timeCounter;
}
