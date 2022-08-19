*my_stragety.h*下新增的内容：

```c++
extern int g_flight_state;	//飞机存活状态
extern int g_cnt_state;		//导弹威胁状态
extern int g_enmy_state;	//敌机数量状态
extern int g_launch_state;	//我方发射导弹状态
extern int g_guide_state;	//我方制导导弹状态

private:
	void MyStrategy::SetFlightState(unsigned int flightID);
	int OutputReward()
```

*my_stragety.cpp*下仅包含了计算*reward*的实现

在飞行控制程序中，每次*action*完成后应调用`int MyStrategy::OutputReward()`，其返回值为当前*state*的*reward*，将其写入*reward.csv*即可。