from tensorflow.keras.models import load_model

import PPO
import global_var
import Environment
from Environment import Env


if __name__ == "__main__":

    """
    初始化
    """

    # 初始化全局变量
    global_var._init()
    global_var.set_value('state', None)
    global_var.set_value('reward', None)
    global_var.set_value('done', False)
    global_var.set_value('state_signal', False)
    global_var.set_value('reward_signal', False)

    # 初始化环境 获取状态空间(state)的维度和动作(action)数量
    env = Env()
    observation_dimensions = env.observation_space.shape
    num_actions = env.action_space.n

    # 初始化actor critic模型
    actor = load_model('actor.h5')
    critic = load_model('critic.h5')

    # 初始化状态(state)
    observation, terminal = env.reset(), global_var.get_value('done')

    """
    启动决策
    """

    while terminal is False:
        # 获取logits action 并在环境中更新一步
        observation = observation.reshape(1, -1)
        logits, action = PPO.sample_action(observation)
        observation_new, reward, done, _ = env.step(action[0].numpy())

        # 输出每个epoch的平均return和平均长度
        print(
            f" Last State: {observation}. Action: {Environment.ACTION_LIST[action]}. State: {observation_new}. Reward: {reward}"
        )

    print("Game Finished")
