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
        model = PPO.load("unreal_PPO_cartpole")
        model.set_env(env)
        print(f"loaded")
    except Exception as e:
        print(e)
        #model = PPO("MlpPolicy", env=env, device="cuda", learning_starts=1000, learning_rate=0.0023, gradient_steps=128, target_update_interval=1000, gamma=0.95)
        model = PPO("MlpPolicy", env=env, device="cuda", learning_rate=0.002)
        print(f"new model")

    new_logger = configure(folder="./log", format_strings =["stdout", "tensorboard"])
    model.set_logger(new_logger)
    model.learn(total_timesteps=10000,tb_log_name='UE_AnimalSimulator', log_interval=1)
    # act = DQN.learn(
    #     env,
    #     network='mlp',
    #     lr=1e-3,
    #     total_timesteps=100000,
    #     buffer_size=50000,
    #     exploration_fraction=0.1,
    #     exploration_final_eps=0.02,
    #     print_freq=10,
    #     callback=callback
    # )
    print("Saving model to unreal_PPO.zip")
    model.save("unreal_PPO")


if __name__ == '__main__':
    main()
