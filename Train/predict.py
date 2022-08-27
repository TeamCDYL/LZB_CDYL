import os
import time
import threading
import numpy as np
import pandas as pd
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler
from tensorflow.keras.models import load_model
import tensorflow as tf


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
CONFIG = r'Config.txt'  # 配置文件目录
ACTOR_MODEL = r'actor.h5'   # actor模型文件目录

ASSIST_WATCH_PATH = r'./'   # 比赛开始标志文件监控目录
DATA_WATCH_PATH = '/Data'  # 数据监控文件目录后半部分
STATE_FILE = '/state.csv'     # state.csv后半部分
ACTION_FILE = '/action.csv'   # action.csv后半部分

# 全局变量
state_signal_1 = False
state_signal_2 = False
race_state_1 = 'wait'
race_state_2 = 'wait'
terminal = False
state_path_1 = r''
state_path_2 = r''
action_path_1 = r''
action_path_2 = r''
watch_path_1 = r''
watch_path_2 = r''
flight_id_1 = 0
flight_id_2 = 0


# action输出转换
def action_transfer(action):
    if action >= 13:
        return '\n1,' + str(action - 13)
    else:
        return '\n0,' + str(action)


# 使用actor采样行动
@tf.function
def sample_action(observation):
    logits = actor(observation)
    action = tf.squeeze(tf.random.categorical(logits, 1), axis=1)
    return logits, action


# 读取配置信息
def read_config():
    global state_path_1
    global state_path_2
    global action_path_1
    global action_path_2
    global watch_path_1
    global watch_path_2
    global flight_id_1
    global flight_id_2
    txt = []

    with open(CONFIG, "r") as f:
        for line in f:
            txt.append(line.strip())
    flight_id_1 = txt[3][txt[3].find('=') + 2]
    flight_id_2 = txt[5][txt[5].find('=') + 2]
    watch_path_1 = watch_path_1 + '../' + 'ACAS_0' + flight_id_1 + DATA_WATCH_PATH
    watch_path_2 = watch_path_2 + '../' + 'ACAS_0' + flight_id_2 + DATA_WATCH_PATH
    state_path_1 = state_path_1 + watch_path_1 + STATE_FILE
    state_path_2 = state_path_2 + watch_path_2 + STATE_FILE
    action_path_1 = action_path_1 + watch_path_1 + ACTION_FILE
    action_path_2 = action_path_2 + watch_path_2 + ACTION_FILE

    print("[SYSTEM] 监控路径1为: %s " % os.path.abspath(watch_path_1))
    print("[SYSTEM] state_1路径为: %s " % os.path.abspath(state_path_1))
    print("[SYSTEM] action_1路径为: %s " % os.path.abspath(action_path_1))
    print("[SYSTEM] 监控路径2为: %s " % os.path.abspath(watch_path_2))
    print("[SYSTEM] state_2路径为: %s " % os.path.abspath(state_path_2))
    print("[SYSTEM] action_2路径为: %s " % os.path.abspath(action_path_2))
    return


# 监控state变化(reward追加至最后一列)
def monitor_state(flight_id):
    if flight_id == 1:
        data = pd.read_csv(state_path_1)
    else:
        data = pd.read_csv(state_path_2)
    state = data.iloc[-1]
    state = np.array(list(map(float, state[1:])), dtype=np.float32)
    reward = state[-1]
    state = state[:-1]
    print('[Data] state: ', state)
    print('[Data] reward: ', reward)
    return state, reward


# 获取环境初值
def reset(flight_id):
    global race_state_1
    global race_state_2
    global state_path_1
    global state_path_2

    if flight_id == 1:
        while race_state_1 != 'fight':
            time.sleep(0.01)
        state_path = state_path_1
    else:
        while race_state_2 != 'fight':
            time.sleep(0.01)
        state_path = state_path_2

    data = pd.read_csv(state_path).iloc[0]
    state = np.array(list(map(float, data[1:-1])), dtype=np.float32)
    return state


# 环境更新
def step(action, flight_id):
    global action_path_1
    global action_path_2
    global state_signal_1
    global state_signal_2
    global terminal

    if flight_id == 1:
        action_file = action_path_1
    else:
        action_file = action_path_2

    with open(action_file, "a") as f:
        f.write(action_transfer(action))
        f.flush()

    if flight_id == 1:
        while not state_signal_1:
            time.sleep(0.01)
    else:
        while not state_signal_2:
            time.sleep(0.01)

    state, reward = monitor_state(flight_id)

    if flight_id == 1:
        state_signal_1 = False
    else:
        state_signal_2 = False

    terminal = True

    print('[RACE] 采取行动为：' + str(ACTION_LIST[action]) + '    回报为：' + str(reward))

    return state


def strategy(flight_id):
    # 初始化状态(state)
    observation = reset(flight_id=flight_id)
    while terminal is False:
        # 获取logits action 并在环境中更新一步
        observation = observation.reshape(1, -1)
        logits, action = sample_action(observation)
        observation = step(action[0].numpy(), flight_id=flight_id)
    return


# watchdog文件监控器重写
class FileEventHandler(FileSystemEventHandler):
    def __init__(self):
        FileSystemEventHandler.__init__(self)

    def on_modified(self, event):
        global state_signal_1
        global state_signal_2
        if not event.is_directory:
            file_path, file_name = os.path.split(event.src_path)
            if file_name == 'state1.csv':
                print("[SYSTEM] state_1更新: %s " % event.src_path)
                state_signal_1 = True
            if file_name == 'state2.csv':
                print("[SYSTEM] state_2更新: %s " % event.src_path)
                state_signal_2 = True

    def on_created(self, event):
        global terminal
        global race_state_1
        global race_state_2
        if not event.is_directory:
            file_path, file_name = os.path.split(event.src_path)
            if file_name == 'end':
                terminal = True
                print('[RACE] 比赛结束')
            if file_name == 'start':
                terminal = False
                print('[RACE] 比赛开始')
            if file_path == watch_path_1 and file_name == 'fight':
                print('[RACE] 1号进入作战状态')
                race_state_1 = 'fight'
            if file_path == watch_path_1 and file_name == 'outfight':
                print('[RACE] 1号进入脱战状态')
                race_state_1 = 'outfight'
            if file_path == watch_path_2 and file_name == 'fight':
                print('[RACE] 2号进入作战状态')
                race_state_1 = 'fight'
            if file_path == watch_path_2 and file_name == 'outfight':
                print('[RACE] 2号进入脱战状态')
                race_state_1 = 'outfight'


if __name__ == "__main__":

    """
    初始化
    """

    # 初始化actor模型
    try:
        actor = load_model('actor.h5')
    except IOError:
        print("[WARNING] 模型未找到")
    else:
        print("[SYSTEM] 成功加载模型")

    # 读取配置文件
    read_config()

    # 插入监控器
    observer = Observer()
    dirs = [watch_path_1, watch_path_2, ASSIST_WATCH_PATH]
    for dir in dirs:
        event_handler = FileEventHandler()
        observer.schedule(event_handler, dir, True)
    observer.start()

    """
    启动决策
    """

    plane_thread_1 = threading.Thread(target=strategy, args=(flight_id_1,))
    plane_thread_2 = threading.Thread(target=strategy, args=(flight_id_2,))
    plane_thread_1.start()
    plane_thread_2.start()

    print("[RACE] Game Finished")

    plane_thread_1.join()
    plane_thread_2.join()
    observer.join()
