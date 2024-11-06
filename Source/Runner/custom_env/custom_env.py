import gymnasium as gym
import socket
import struct

HOST = '127.0.0.1'
PORT = 7794

class custom_env(gym.Env):
    def __init__(self):
        self.action_space = gym.spaces.Box(-1,1,(2,))
        self.observation_space = gym.spaces.Box(0, 10, (8,))

        print(self.observation_space.shape)

        self.hasConnection = False
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        try:
            self.socket.connect((HOST, PORT))
            print("successfully connected")
            self.hasConnection = True
        except socket.error:
            print("couldn't connect" + socket.error)

    def step(self, action):

        #print("action: ",action)
        data = struct.pack('fff',action[0],action[1],0)
        self.socket.send(data)

        state = (1,1,0,0,0,0,0,0)

        # wait till you get a response
        while(True):
            data = self.socket.recv(32)
            if not data:
                break

            state = struct.unpack('ffffffff',data)
            #print(state)

            break
    
        reward = state[0] #health percentage
        
        done = False
        if(state[0] < 0): #health zero
            done = True

        info = {}
        return state, reward, done, False ,info

    def reset(self, seed=None, options = None):
        data = struct.pack('fff',0,0,1)
        self.socket.send(data)
        #print("reset")

        state = (1,1,0,0,0,0,0,0)

        # wait till you get a response
        while(True):
            data = self.socket.recv(32)
            if not data:
                break
            #state = int.from_bytes(data, byteorder='big', signed=True)
            state = struct.unpack('ffffffff',data)
            #print(state)
            break
        info = {}
        return state, info