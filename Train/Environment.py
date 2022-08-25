import time
import csv
import collections
import numpy as np
import pandas as pd
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler
from tensorflow.keras.models import load_model

import global_var

# 状态维度和动作集
STATE_DIM = 14
ACTION_LIST = {
    0: 'DoTacHeadEvade',
    1: 'DoTacPointAtk',
    2: 'DoTacToTar',
    3: 'DoTacAltClimbP30',
    4: 'DoTacAltClimbP60',
    5: 'DoTacNoseDiveM30',
    6: 'DoTacNoseDiveM600',
    7: 'DoTacStaHov',
    8: 'DoTurnLeft30',
    9: 'DoTurnLeft60',
    10: 'DoTurnRight30',
    11: 'DoTacHeadEvade',
    12: 'DoTurnEvad30',
    13: 'DoTurnEvad60',
    14: 'DoTacWpnShoot',
    15: 'SwitchGuideFlight'
}

# 文件路径
STATE_FILE = 'state1.csv'
ACTION_FILE = 'action1.csv'
REWARD_FILE = 'reward1.csv'
WATCH_PATH = 'E:/Project/LZB_611/LZB_CDYL/ACAIStrategyDemo/dist'  # 监控目录


# 插入监控器
def set_watchdog():
    event_handler = FileMonitorHandler()
    observer = Observer()
    observer.schedule(event_handler, path=WATCH_PATH, recursive=True)  # recursive递归的
    observer.start()
    observer.join()


# 监控state变化
def monitor_state():
    with open(STATE_FILE, "r") as f:
        reader = csv.reader(f)
        data = list(reader)
        state = data[-1]
    state = np.array(list(map(int, state)), dtype=np.float32)
    global_var.set_value('state', state)
    global_var.set_value('state_signal', True)


# 监控reward变化
def monitor_reward():
    with open(REWARD_FILE, "r") as f:
        reader = csv.reader(f)
        data = list(reader)
        reward = data[-1]
    reward = int(reward)
    global_var.set_value('reward', reward)
    global_var.set_value('reward_signal', True)


# watchdog文件监控器重写
class FileMonitorHandler(FileSystemEventHandler):
    def __init__(self, **kwargs):
        super(FileMonitorHandler, self).__init__(**kwargs)
        self._watch_path = WATCH_PATH

    def on_modified(self, event):
        if not event.is_directory:
            file_path = event.src_path
            if file_path[-13:] == 'state_run.csv':
                print("state更新: %s " % file_path)
                monitor_state()
            if file_path[-14:] == 'reward_run.csv':
                print("reward更新: %s " % file_path)
                monitor_reward()

    def on_created(self, event):
        if not event.is_directory:
            file_path = event.src_path
            if file_path[-4:] == 'true':
                print('比赛终止')
                global_var.set_value('done', True)


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
            f.write('\n' + action)

        while not global_var.get_value('state_signal'):
            time.sleep(0.01)
        self.state = global_var.get_value('state')
        global_var.set_value('state_signal', False)

        while not global_var.get_value('reward_signal'):
            time.sleep(0.01)
        reward = global_var.get_value('reward')
        global_var.set_value('reward_state', False)

        done = global_var.get_value('done')

        return self.state, reward, done, {}

    # 环境重置
    def reset(self):
        with open(STATE_FILE, "r") as f:
            reader = csv.reader(f)
            data = list(reader)
            state = data[0]
        self.state = np.array(list(map(int, state)), dtype=np.float32)
        set_watchdog()
        return self.state
