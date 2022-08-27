import time
import csv
import numpy as np
import pandas as pd
from watchdog.events import FileSystemEventHandler

import global_var

# 状态维度和动作集
STATE_DIM = 16
ACTION_LIST = {
    0: 'DoTacPonitAtk',
    1: 'DoTacToTar',
    2: 'DoTacAltClimbP30',
    3: 'DoTacAltClimbP60',
    4: 'DoTacNoseDiveM30',
    5: 'DoTacNoseDiveM600',
    6: 'DoTacStaHov',
    7: 'DoTurnLeft30',
    8: 'DoTurnLeft60',
    9: 'DoTurnRight30',
    10: 'DoTacHeadEvade',
    11: 'DoTurnEvad30',
    12: 'DoTurnEvad60',
    13: 'DoTacWpnShoot',
    14: 'SwitchGuideFlight'
}

# 文件路径
STATE_FILE = r'../ACAIStrategyDemo/dist/state1.csv'
ACTION_FILE = r'../ACAIStrategyDemo/dist/action1.csv'
WATCH_PATH = r'../ACAIStrategyDemo/dist'  # 监控目录


# 监控state变化(reward追加至最后一列)
def monitor_state():
    data = pd.read_csv(STATE_FILE)
    state = data.iloc[-1]
    state = np.array(list(map(float, state[1:])), dtype=np.float32)
    reward = state[-1]
    state = state[:-1]
    print('state: ', state)
    print('reward: ', reward)
    return state, reward


# action输出转换
def action_transfer(action):
    if action >= 13:
        return '\n1,' + str(action - 13)
    else:
        return '\n0,' + str(action)


# watchdog文件监控器重写
class FileMonitorHandler(FileSystemEventHandler):
    def __init__(self, **kwargs):
        super(FileMonitorHandler, self).__init__(**kwargs)
        self._watch_path = WATCH_PATH

    def on_modified(self, event):
        if not event.is_directory:
            file_path = event.src_path
            if file_path[-10:] == STATE_FILE[-10:]:
                print("[SYSTEM] state更新: %s " % file_path)
                global_var.set_value('state_signal', True)

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
                print('[RACE] 进入作战状态')
                global_var.set_value('race_state', 'fight')
            if file_path[-8:] == 'outfight':
                print('[RACE] 进入脱战状态')
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

    def __init__(self):
        self.action_space = ActionSpace(ACTION_LIST)
        self.observation_space = StateSpace(STATE_DIM)
        self.state = self.observation_space.state

    # 环境更新
    def step(self, action):
        with open(ACTION_FILE, "a") as f:
            f.write(action_transfer(action))
            f.flush()

        while not global_var.get_value('state_signal'):
            time.sleep(0.01)
        self.state, reward = monitor_state()
        global_var.set_value('state_signal', False)

        done = global_var.get_value('done')

        print('[RACE] 采取行动为：' + str(ACTION_LIST[action]) + '    回报为：' + str(reward))

        return self.state, reward, done, {}

    # 环境重置
    def reset(self):
        while global_var.get_value('race_state') != 'fight':
            time.sleep(0.01)
        data = pd.read_csv(STATE_FILE)
        state = data.iloc[0]
        self.state = np.array(list(map(float, state[1:-1])), dtype=np.float32)
        return self.state
