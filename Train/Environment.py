import time
import numpy as np
import pandas as pd
from watchdog.events import FileSystemEventHandler

import global_var

# 友机距离对数 友机高度差对数 友机速度矢量夹角
# lead速度(100m/s) 是否被导弹锁定 是否在制导 剩余武器量 與終點距離对数 目标距离对数 目标高度差对数 目标地理系方位角 目标速度(100m/s)
# wing速度(100m/s) 是否被导弹锁定 是否在制导 剩余武器量 與終點距離对数 目标距离对数 目标高度差对数 目标地理系方位角 目标速度(100m/s)
# 扫描到的敌机数量 场上我方导弹数量 剩餘敵機數量

# 状态维度和动作集
STATE_DIM = 18
ACTION_LIST = {
    0: 'DoTacCir',
    1: 'DoTacToTar',
    2: 'DoTacAlt0.5',
    3: 'DoTacAlt0.4',
    4: 'DoTacAlt0.3',
    5: 'DoTacAlt0.2',
    6: 'DoTacStaHov',
    7: 'DoTurnLeft30',
    8: 'DoTurnLeft60',
    9: 'DoTurnRight30',
    10: 'DoTurnRight60',
    11: 'DoTacHeadEvade',
    12: 'DoTacAlt0.6',
    13: 'DoTacPointAtk',
    14: 'DoTurnFor'
}

# 文件路径
STATE_1_FILE = r'./Data/Lead/state1.csv'
ACTION_1_FILE = r'./Data/Lead//action1.csv'
STATE_2_FILE = r'./Data/Wing/state2.csv'
ACTION_2_FILE = r'./Data/Wing/action2.csv'
WATCH_PATH = r'./Data'  # 监控目录


# 监控state变化(reward追加至最后一列)
def monitor_state(state_file):
    data = pd.read_csv(state_file)
    state = data.iloc[-1]
    state = np.array(list(map(float, state[1:])), dtype=np.float32)
    reward = state[-1]
    state = state[:-1]
    print('[Data] state: ', state)
    print('[Data] reward: ', reward)
    return state, reward


# action输出转换
def action_transfer(action):
    if action >= 10:
        return '1,' + str(action - 10) + '\n'
    else:
        return '0,' + str(action) + '\n'


# watchdog文件监控器重写
class FileMonitorHandler(FileSystemEventHandler):
    def __init__(self, **kwargs):
        super(FileMonitorHandler, self).__init__(**kwargs)
        self._watch_path = WATCH_PATH

    def on_modified(self, event):
        if not event.is_directory:
            file_path = event.src_path
            if file_path[-10:] == STATE_1_FILE[-10:]:
                print("[SYSTEM] lead_state更新: %s " % file_path)
                global_var.set_value('state_signal_1', True)
            if file_path[-10:] == STATE_2_FILE[-10:]:
                print("[SYSTEM] wing_state更新: %s " % file_path)
                global_var.set_value('state_signal_2', True)

    def on_created(self, event):
        if not event.is_directory:
            file_path = event.src_path
            if file_path[-3:] == 'end':
                global_var.set_value('done', True)
                print('[RACE] 第{times}场比赛结束'.format(times=global_var.get_value('game_times')))
            if file_path[-5:] == 'start':
                global_var.set_value('done', False)
                global_var.set_value('game_times', global_var.get_value('game_times') + 1)
                print('[RACE] 第{times}场比赛开始'.format(times=global_var.get_value('game_times')))
            if file_path[-5:] == 'fight':
                print('[RACE] 当前状态为作战状态')
                global_var.set_value('done', False)
                global_var.set_value('race_state', 'fight')
            if file_path[-5:] == 'outft':
                print('[RACE] 当前状态为脱战状态')
                global_var.set_value('race_state', 'outfight')


class StateSpace(object):
    # 状态类
    shape = 0
    state = np.empty(shape=shape)

    def __init__(self, dim):
        self.shape = dim
        self.state = np.empty(shape=self.shape)


class ActionSpace(object):
    # 动作类
    action_list = {}
    n = 0

    def __init__(self, action_list):
        self.action_list = action_list
        self.n = len(self.action_list)


class Env(object):
    # 环境类
    observation_space = None
    action_space = None
    state = None
    state_file = None
    action_file = None
    role = ""

    def __init__(self, state_file, action_file, role):
        self.action_space = ActionSpace(ACTION_LIST)
        self.observation_space = StateSpace(STATE_DIM)
        self.state = self.observation_space.state
        self.state_file = state_file
        self.action_space = action_file
        self.role = role

    # 环境更新
    def step(self, action):
        sign = ""
        if self.role == "lead":
            sign = "state_signal_1"
        elif self.role == "wing":
            sign = "state_signal_2"
        with open(self.action_file, "a") as f:
            f.write(action_transfer(action))
            f.flush()

        while not global_var.get_value(sign):
            time.sleep(0.01)
        self.state, reward = monitor_state(self.state_file)
        global_var.set_value(sign, False)

        done = global_var.get_value('done')

        print('[RACE] 采取行动为：' + str(ACTION_LIST[action]) + '    回报为：' + str(reward) + '     终止否：' + str(done))

        return self.state, reward, done, {}

    # 环境重置
    def reset(self):
        while global_var.get_value('race_state') != 'fight':
            time.sleep(0.01)
        data = pd.read_csv(self.state_file)
        state = data.iloc[0]
        self.state = np.array(list(map(float, state[1:-1])), dtype=np.float32)
        return self.state
