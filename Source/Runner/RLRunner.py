import gymnasium as gym
from stable_baselines3 import DQN, PPO
from stable_baselines3.common.logger import configure
from custom_env import custom_env

def callback(lcl, _glb):
    # stop training if reward exceeds 199
    is_solved = lcl['t'] > 100000 and sum(lcl['episode_rewards'][-101:-1]) / 100 >= 1000
    return is_solved

def main():
    env = custom_env.custom_env()
    try :
        model = PPO.load("PreyModel2")
        model.set_env(env)
        print(f"loaded")
    except Exception as e:
        print(e)
        model = PPO("MlpPolicy", env=env, device="cuda", learning_rate=0.001)
        print(f"new model")

    new_logger = configure(folder="./log/Log_PreyModel_2", format_strings =["stdout", "tensorboard"])
    model.set_logger(new_logger)
    model.learn(total_timesteps=50000,tb_log_name='Log_PreyModel', log_interval=1)

    print("Saving model to PreyModel.zip")
    model.save("PreyModel2")


if __name__ == '__main__':
    main()
