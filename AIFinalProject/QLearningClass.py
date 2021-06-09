import mdpmodified as mdp
import numpy as _np
#Found at https://github.com/gkhayes/maze_reinforcement_learning using her implementation of the matrices and basis for the implementation
# of the QLearing algorithim as well as the showing which files of the mdptoolbox libray needed to be modified to function for this problem


# Create transition and reward matrices 
def createMatrices(maze, reward, penalty_s, penalty_l, prob):
    #r = row, c = column
    rows, columns = _np.shape(maze)
    states = rows*columns
    p = prob
    q = (1 - prob)*0.5
    
    #Reward matrix
    path = maze*penalty_s #penalty for entering a valid cell in the maze aka 1
    walls = (1 - maze)*penalty_l #penalty for entering an invalid cell in the maze aka 0
    combined = path + walls
    
    combined[-1, -1] = reward
    #R = Reward Matrix        
    Reward = _np.reshape(combined, states)
    
    #Transition matrix
    T_up = _np.zeros((states, states))
    T_left = _np.zeros((states, states))
    T_right = _np.zeros((states, states))
    T_down = _np.zeros((states, states))
    #Checks to see if we are in a wall/ 0
    wall_ind = _np.where(Reward == penalty_l)[0]

    for i in range(states):
        #Move Up
        if (i - columns) < 0 or (i - columns) in wall_ind :
            T_up[i, i] += p
        else:
            T_up[i, i - columns] += p
        
        if i%columns == 0 or (i - 1) in wall_ind:
            T_up[i, i] += q
        else:
            T_up[i, i-1] += q
        
        if i%columns == (columns - 1) or (i + 1) in wall_ind:
            T_up[i, i] += q
        else:
            T_up[i, i+1] += q
            
        #Move Down
        if (i + columns) > (states - 1) or (i + columns) in wall_ind:
            T_down[i, i] += p
        else:
            T_down[i, i + columns] += p
        
        if i%columns == 0 or (i - 1) in wall_ind:
            T_down[i, i] += q
        else:
            T_down[i, i-1] += q
        
        if i%columns == (columns - 1) or (i + 1) in wall_ind:
            T_down[i, i] += q
        else:
            T_down[i, i+1] += q
            
        #Move Left
        if i%columns == 0 or (i - 1) in wall_ind:
            T_left[i, i] += p
        else:
            T_left[i, i-1] += p
            
        if (i - columns) < 0 or (i - columns) in wall_ind:
            T_left[i, i] += q
        else:
            T_left[i, i - columns] += q
        
        if (i + columns) > (states - 1) or (i + columns) in wall_ind:
            T_left[i, i] += q
        else:
            T_left[i, i + columns] += q
        
        #Move Right
        if i%columns == (columns - 1) or (i + 1) in wall_ind:
            T_right[i, i] += p
        else:
            T_right[i, i+1] += p
            
        if (i - columns) < 0 or (i - columns) in wall_ind:
            T_right[i, i] += q
        else:
            T_right[i, i - columns] += q
        
        if (i + columns) > (states - 1) or (i + columns) in wall_ind:
            T_right[i, i] += q
        else:
            T_right[i, i + columns] += q
    
    T = [T_up, T_left, T_right, T_down] 
    
    return T, Reward
#ALL code above if from GKHayes all it does is define movements for the exploration process



def main():
    # Define maze array
    maze =  _np.array([
    [ 1.,  1.,  1.,  1.,  1.,  1.,  1.],
    [ 0.,  0.,  1.,  0.,  0.,  1.,  0.],
    [ 0.,  0.,  0.,  0.,  1.,  1.,  0.],
    [ 1.,  1.,  1.,  1.,  1.,  0.,  1.],
    [ 0.,  1.,  0.,  0.,  1.,  1.,  1.],
    [ 1.,  0.,  1.,  1.,  1.,  1.,  1.],
    [ 1.,  1.,  1.,  0.,  1.,  1.,  1.]])
        
    # Create transition and reward matrices
    T, R = createMatrices(maze, 1, -0.04, -0.75,  0.8)
    
    # Set q-learning parameters
    gamma = 0.9 # Discount factor
    alpha = 0.4 # Learning rate
    randomAction = 0.45 # Random action prob
    decay = 1.05 # Rate of decay
    iters = 500000 # Number of iterations
    
    # Run Q-learning algorithm to find optimal policy
    _np.random.seed(1)
    q = mdp.QLearning(T, R, gamma, alpha, randomAction, decay, iters)
    q.run()
    
    # Print optimal policy
    pol = _np.reshape(_np.array(list(q.policy)), _np.shape(maze))
    pol = pol.astype('str')
    pol[pol == "0"] = "Up"
    pol[pol == "1"] = "Left"
    pol[pol == "2"] = "Right"
    pol[pol == "3"] = "Down"
    print(pol)


main()