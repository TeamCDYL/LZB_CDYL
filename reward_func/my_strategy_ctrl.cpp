#include "my_strategy.h"
#include "string.h"

int g_flight_state = 0;
int g_cnt_state = 0;
int g_enmy_state = 0;
int g_launch_state = 0;
int g_guide_state = 0;

void MyStrategy::onPKOut(unsigned int flightID)
{
	SetFlightState(flightID);
}

void MyStrategy::SetFlightState(unsigned int flightID)
{
	// 判断友方（或者自身）飞机被击落的情况，返回状态值2
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

	// 判断敌方飞机被击落的情况，返回状态值1
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

int MyStrategy::OutputReward()
{
	int reward = 0;

	// 敌方飞机被击落的情况（击落一架敌机）
	if(g_flight_state == 1)
	{
		// 重置状态变量
		g_flight_state = 0;
		reward += 250;
	}
	// 友方飞机或自身被击落的情况（友方飞机被击落）
	if(g_flight_state == 2)
	{
		// 重置状态变量
		g_flight_state = 0;
		reward -= 250;
	}
	// 当导弹威胁数增加且没有飞机被击中时的reward（我方被敌方导弹锁定）
	if (g_cnt_state < mACMslWarning.mslCnt && g_flight_state == 0 && mACFlightStatus.flightID == mACMslWarning.flightID)
	{
		g_cnt_state++;
		reward -= 5;
	}
	// 当导弹威胁数减少并且没有飞机坠毁时的reward（我方逃脱敌方导弹）
	if (g_cnt_state > mACMslWarning.mslCnt && g_flight_state == 0 && mACFlightStatus.flightID == mACMslWarning.flightID)
	{
		g_cnt_state--;
		reward += 10;
	}
	// 新侦查到敌机的情况（我方获得敌方视野）
	if (g_enmy_state < mACRdrTarget.tgtCnt && g_flight_state == 0)
	{
		g_enmy_state++;
		reward += 2;
	}
	// 侦察到的敌机数量减少（我方丢失敌方视野）
	if (g_enmy_state > mACRdrTarget.tgtCnt && g_flight_state == 0)
	{
		g_enmy_state--;
		reward -= 2;
	}
	// 飞机发射导弹
	if (g_launch_state < mTeamMSLLaunched.mslCnt && g_flight_state == 0 && mTeamMSLLaunched.trajectoryInfos[0].launchFlightID == mACFlightStatus.flightID)
	{
		g_launch_state++;
		reward -= 10;
	}
	// 我方导弹锁定敌方一架飞机
	if (g_guide_state < mACMSLInGuide.mslCnt && g_flight_state == 0 && mACMSLInGuide.guideFlightID == mACFlightStatus.flightID)
	{
		g_guide_state++;
		reward += 5;
	}
	return reward;
}