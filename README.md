Reinforcement Learning Model For Animating Prey-Predator Behavior

Developed with Unreal Engine 5 & Stable BaseLine + Gymnasium

# Requirement
gymnasium, stable baseline 3 for Runner

# Description
Basically Unreal Engine works as Environment and Actor.
RLRunner.py is brain of this actor. It receives observation from unreal actor and process it with RL to decide next move for actor.
The output action is then transferred to Unreal Engine and move the pawn with corresponding info.

# How to Run
First run the game with unreal engine and then run ./Source/Runner RLRunner.py
after connection is established it will automatically learn from it's action and save RL model to python file's location.
You can change total learning timestep by RLRunner.py.
You can continue learning model when there's already model file in it's location.
