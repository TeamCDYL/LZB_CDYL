import sys
import time
import numpy as np
import tensorflow as tf
from tensorflow import keras

import os

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


# 使用actor采样行动
@tf.function
def sample_action(observation, actor):
    logits = actor(observation)
    action = tf.squeeze(tf.random.categorical(logits, 1), axis=1)
    return logits, action


if __name__ == "__main__":

    #print(os.path.abspath(__file__))

    flight_role = int(sys.argv[1])
    state = np.array(list(map(float, sys.argv[3:])), dtype=np.float32)
    #
    state = state.reshape(1, -1)
    print(state.shape)
    model = keras.models.Model()
    if flight_role == 1:
        try:
            lead_model = keras.models.load_model('actor_lead.h5', compile=False)
        except IOError:
            print("[WARNING] lead模型未找到，请检查模型文件路径并重启此程序")
            time.sleep(5)
            sys.exit()
        else:
            model = lead_model
            print("[INFO] 成功加载lead模型")
    elif flight_role == 2:
        try:
            lead_model = keras.models.load_model('Assist/actor_wing.h5', compile=False)
        except IOError:
            print("[WARNING] wing模型未找到，请检查模型文件路径并重启此程序")
            time.sleep(5)
            sys.exit()
        else:
            model = lead_model
            print("[WARNING] 成功加载wing模型")
    else:
        print("[WARNING] 模型加载失败，请检查命令行参数")

    logits, action = sample_action(state, model)
    action = action[0].numpy()
    print(f"[INFO] 采取行动为{ACTION_LIST[action]}")
    exit(action)
