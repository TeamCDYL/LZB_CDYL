import time
import numpy as np
import tensorflow as tf
from tensorflow import keras
from keras import layers
import scipy.signal
from watchdog.observers import Observer

import Environment
from Environment import Env, WATCH_PATH
from Environment import FileMonitorHandler
import global_var


"""
训练参数
"""

steps_per_epoch = 5
epochs = 10
gamma = 0.99
clip_ratio = 0.2
policy_learning_rate = 3e-4
value_function_learning_rate = 1e-3
train_policy_iterations = 5
train_value_iterations = 5
lam = 0.97
target_kl = 0.01
hidden_sizes = (128, 64, 16)


"""
函数和类
"""


def discounted_cumulative_sums(x, discount):
    # 计算回报值和优势估计值向量的折现累积总和
    return scipy.signal.lfilter([1], [1, float(-discount)], x[::-1], axis=0)[::-1]


class Buffer(object):
    # 数据缓冲池
    def __init__(self, observation_dimensions, size, gamma=0.99, lam=0.95):
        # 初始化

        # 参数
        self.gamma, self.lam = gamma, lam

        # 状态 state
        self.observation_buffer = np.zeros((size, observation_dimensions), dtype=np.float32)
        # 动作 action
        self.action_buffer = np.zeros(size, dtype=np.int32)
        # 回报 reward
        self.reward_buffer = np.zeros(size, dtype=np.float32)

        # 优势函数 advantage
        self.advantage_buffer = np.zeros(size, dtype=np.float32)
        # 价值函数 value
        self.value_buffer = np.zeros(size, dtype=np.float32)
        # 累计回报函数 return
        self.return_buffer = np.zeros(size, dtype=np.float32)

        # 对数几率
        self.log_probability_buffer = np.zeros(size, dtype=np.float32)

        # 池顶指针 TD误差采样轨迹开始指针
        self.pointer, self.trajectory_start_index = 0, 0

    def store(self, observation, action, reward, value, log_probability):
        # 存储新的数据到缓冲池中

        self.observation_buffer[self.pointer] = observation
        self.action_buffer[self.pointer] = action
        self.reward_buffer[self.pointer] = reward
        self.value_buffer[self.pointer] = value
        self.log_probability_buffer[self.pointer] = log_probability
        self.pointer += 1

    def get(self):
        # 获取缓冲池中的所有数据 将优势归一化

        self.pointer, self.trajectory_start_index = 0, 0

        advantage_mean, advantage_std = (
            np.mean(self.advantage_buffer),
            np.std(self.advantage_buffer),
        )
        if advantage_std == 0:
            advantage_std = 1
        self.advantage_buffer = (self.advantage_buffer - advantage_mean) / advantage_std
        return (
            self.observation_buffer,
            self.action_buffer,
            self.advantage_buffer,
            self.return_buffer,
            self.log_probability_buffer,
        )

    def finish_trajectory(self, last_value=0):
        # 通过优势估计和回报来完成采样轨迹

        # 采样轨迹的切片方式
        path_slice = slice(self.trajectory_start_index, self.pointer)

        rewards = np.append(self.reward_buffer[path_slice], last_value)
        values = np.append(self.value_buffer[path_slice], last_value)

        # TD误差
        deltas = rewards[:-1] + self.gamma * values[1:] - values[:-1]

        self.advantage_buffer[path_slice] = discounted_cumulative_sums(deltas, self.gamma * self.lam)
        self.return_buffer[path_slice] = discounted_cumulative_sums(rewards, self.gamma)[:-1]

        self.trajectory_start_index = self.pointer


def mlp(x, sizes, activation=tf.tanh, output_activation=None):
    # 构建前馈神经网络
    for size in sizes[:-1]:
        x = layers.Dense(units=size, activation=activation)(x)
    return layers.Dense(units=sizes[-1], activation=output_activation)(x)


def log_probabilities(logits, a):
    # 将actor输出结果转换为采取各个的行动对数几率
    log_probabilities_all = tf.nn.log_softmax(logits)
    log_probability = tf.reduce_sum(
        tf.one_hot(a, num_actions) * log_probabilities_all, axis=1
    )
    return log_probability


# 使用actor采样行动
@tf.function
def sample_action(observation):
    logits = actor(observation)
    action = tf.squeeze(tf.random.categorical(logits, 1), axis=1)
    return logits, action


# 目标函数 最大化PPO-Clip
@tf.function
def train_policy(observation_buffer, action_buffer, log_probability_buffer, advantage_buffer):

    with tf.GradientTape() as tape:
        ratio = tf.exp(
            log_probabilities(actor(observation_buffer), action_buffer)
            - log_probability_buffer
        )
        min_advantage = tf.where(
            advantage_buffer > 0,
            (1 + clip_ratio) * advantage_buffer,
            (1 - clip_ratio) * advantage_buffer,
        )

        policy_loss = -tf.reduce_mean(
            tf.minimum(ratio * advantage_buffer, min_advantage)
        )
    policy_grads = tape.gradient(policy_loss, actor.trainable_variables)
    policy_optimizer.apply_gradients(zip(policy_grads, actor.trainable_variables))
    # print("policy_function_loss: " + policy_loss)

    kl = tf.reduce_mean(
        log_probability_buffer
        - log_probabilities(actor(observation_buffer), action_buffer)
    )
    kl = tf.reduce_sum(kl)
    return kl


# 使用均方误差训练值函数
@tf.function
def train_value_function(observation_buffer, return_buffer):
    with tf.GradientTape() as tape:
        value_loss = tf.reduce_mean((return_buffer - critic(observation_buffer)) ** 2)
    value_grads = tape.gradient(value_loss, critic.trainable_variables)
    value_optimizer.apply_gradients(zip(value_grads, critic.trainable_variables))
    # print("value function loss: " + value_loss)


if __name__ == "__main__":

    """
    初始化
    """

    # 初始化全局变量
    global_var._init()
    global_var.set_value('done', False)
    global_var.set_value('state_signal', False)
    global_var.set_value('race_state', 'wait')
    global_var.set_value('count', 0)
    global_var.set_value('game_times', 0)

    # 初始化环境 获取状态空间(state)的维度和动作(action)数量
    env = Env()
    observation_dimensions = env.observation_space.shape
    num_actions = env.action_space.n

    # 初始化缓冲池
    buffer = Buffer(observation_dimensions, steps_per_epoch)

    # 初始化actor和critic的网络模型
    observation_input = keras.Input(shape=(observation_dimensions,), dtype=tf.float32)
    logits = mlp(observation_input, list(hidden_sizes) + [num_actions], tf.tanh, None)
    actor = keras.Model(inputs=observation_input, outputs=logits)
    value = tf.squeeze(mlp(observation_input, list(hidden_sizes) + [1], tf.tanh, None), axis=1)
    critic = keras.Model(inputs=observation_input, outputs=value)

    try:
        actor_model = keras.models.load_model('actor.h5')
        critic_model = keras.models.load_model('critic.h5')
    except IOError:
        print("[SYSTEM] 模型未找到，将在本次训练中重新生成模型")
    else:
        actor = actor_model
        critic = critic_model
        print("[SYSTEM] 成功加载模型")

    # 初始化policy和value的优化器
    policy_optimizer = keras.optimizers.Adam(learning_rate=policy_learning_rate)
    value_optimizer = keras.optimizers.Adam(learning_rate=value_function_learning_rate)

    # 插入监控器
    event_handler = FileMonitorHandler()
    observer = Observer()
    observer.schedule(event_handler, path=WATCH_PATH, recursive=True)  # recursive递归的
    observer.start()

    # 初始化状态(state) 剧集返回值 剧集长度
    episode_return, episode_length = 0, 0
    """
    训练
    """

    for epoch in range(epochs):
        # 等待游戏开始
        while global_var.get_value('done'):
            time.sleep(0.1)

        observation = env.reset()

        # 初始化每个epoch的return值、长度和剧集数量之和
        sum_return = 0
        sum_length = 0
        num_episodes = 0

        # 步进迭代
        for t in range(steps_per_epoch):

            # 获取logits action 并在环境中更新一步
            observation = observation.reshape(1, -1)
            logits, action = sample_action(observation)
            observation_new, reward, done, _ = env.step(action[0].numpy())
            episode_return += reward
            episode_length += 1

            # 获取action的对数几率和当前状态的value
            value_t = critic(observation)
            log_probability_t = log_probabilities(logits, action)

            # 存储 observation, action, reward, value_t, logp_pi_t
            buffer.store(observation, action, reward, value_t, log_probability_t)

            # 获取新的状态
            observation = observation_new

            # 到达设定边界时终止采样轨迹
            terminal = done
            if terminal or (t == steps_per_epoch - 1):
                last_value = 0 if done else critic(observation.reshape(1, -1))
                buffer.finish_trajectory(last_value)
                sum_return += episode_return
                sum_length += episode_length
                num_episodes += 1
                observation, episode_return, episode_length = env.reset(), 0, 0

        # 从缓存池中获取数据
        (
            observation_buffer,
            action_buffer,
            advantage_buffer,
            return_buffer,
            log_probability_buffer,
        ) = buffer.get()

        # 更新策略，根据KL停止更新
        for _ in range(train_policy_iterations):
            kl = train_policy(observation_buffer, action_buffer, log_probability_buffer, advantage_buffer)
            if kl > 1.5 * target_kl:
                break

        # 更新value函数
        for _ in range(train_value_iterations):
            train_value_function(observation_buffer, return_buffer)

        # 保存训练模型
        actor.save('actor.h5')
        critic.save('critic.h5')

        # 输出每个epoch的平均return和平均长度
        print(
            f"[TRAIN] Epoch: {epoch + 1}. Mean Return: {sum_return / num_episodes}. Mean Length: {sum_length / num_episodes}"
        )

    observer.join()
