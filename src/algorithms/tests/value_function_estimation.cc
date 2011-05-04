/* -*- Mode: C++; -*- */
/* VER: $Id: Distribution.h,v 1.3 2006/11/06 15:48:53 cdimitrakakis Exp cdimitrakakis $*/
// copyright (c) 2006 by Christos Dimitrakakis <christos.dimitrakakis@gmail.com>
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef MAKE_MAIN
#include "PolicyEvaluation.h"
#include "ValueIteration.h"
#include "MultiMDPValueIteration.h"
#include "RandomMDP.h"
#include "Gridworld.h"
#include "InventoryManagement.h"
#include "DiscretePolicy.h"
#include "Environment.h"
#include "ExplorationPolicy.h"
#include "Sarsa.h"
#include "QLearning.h"
#include "ModelBasedRL.h"
#include "DiscreteMDPCounts.h"
#include "DiscreteChain.h"
//#include "RandomNumberGenerator.h"
#include "RandomSourceRNG.h"
/** 
    \file value_function_estimation.cc
	
    \brief Test accuracy of value function estimation.
*/


struct Statistics
{
    real total_reward;
    real discounted_reward;
    int steps;
};


std::vector<Statistics> EvaluateAlgorithm(int n_steps,
										  int n_episodes,
										  OnlineAlgorithm<int,int>* algorithm,
										  Environment<int, int>* environment,
										  real gamma);

int main (int argc, char** argv)
{
    int n_actions = 4;
    int n_states = 4;
    real gamma = 0.9;
    real lambda = 0.9;
    real alpha = 0.01;
    real randomness = 0.1;
    real pit_value = 0.0;
    real goal_value = 1.0;
    real step_value = 0.01;
    real epsilon = 0.01;
    int n_episodes = 10;
    int n_steps = 1000;

    if (argc != 6) {
		std::cerr << "Usage: online_algorithms n_states n_actions gamma lambda randomness\n";
		return -1;
    }
    n_states = atoi(argv[1]);
    assert (n_states > 0);

    n_actions = atoi(argv[2]);
    assert (n_actions > 0);

    gamma = atof(argv[3]);
    assert (gamma > 0 && gamma <= 1);

    lambda = atof(argv[4]);
    assert (lambda >= 0 && lambda <= 1);

    randomness = atof(argv[5]);
    assert (randomness >= 0 && randomness <= 1);
    

    std::cout << "Starting test program" << std::endl;
    


    //const DiscreteMDP* mdp = environment->getMDP();
    //assert(n_states == mdp->GetNStates());
    //assert(n_actions == mdp->GetNActions());
	RandomSourceRNG rng(false);
    
    std::cout << "Starting evaluation" << std::endl;

    // remember to use n_runs
	if (1) {
		std::cerr << "Creating environment" << std::endl;
		Environment<int, int>* environment = NULL;
		if (1) {
			environment = new DiscreteChain(n_states);
		} else {
			environment = new Gridworld("/home/olethros/projects/beliefbox/dat/maze1",  8, 8);
		}
		n_states = environment->getNStates();
		n_actions = environment->getNActions();
		
		std::cerr << "Creating exploration policy" << std::endl;
		VFExplorationPolicy* exploration_policy = NULL;
		exploration_policy = new EpsilonGreedy(n_actions, epsilon);
    
		std::cerr << "Creating online algorithm" << std::endl;
		OnlineAlgorithm<int, int>* algorithm = NULL;
		MDPModel* mdp_model = NULL;
		if (1) {
			algorithm = new QLearning(n_states,
									  n_actions,
									  gamma,
									  lambda,
									  alpha,
									  exploration_policy);
		} else {
			mdp_model = new DiscreteMDPCounts(n_states,
											  n_actions,
											  1./((real) n_states));
											  
			algorithm = new ModelBasedRL(n_states,
										 n_actions,
										 gamma,
										 epsilon,
										 mdp_model);

		}

		std::vector<Statistics> run_statistics = EvaluateAlgorithm(n_steps, n_episodes, algorithm, environment, gamma);
		
		for (uint i=0; i!=run_statistics.size(); ++i) {
			std::cout << "R:" << run_statistics[i].total_reward 
					  << " V:" << run_statistics[i].discounted_reward
					  << std::endl;
		}
		delete environment;
		delete algorithm;
		delete mdp_model;
		delete exploration_policy;
    }
    

    std::cout << "Done" << std::endl;


    
    return 0;
}

std::vector<Statistics> EvaluateAlgorithm(int n_steps,
										  int n_episodes,
										  OnlineAlgorithm<int, int>* algorithm,
										  Environment<int, int>* environment,
										  real gamma)
{
	real accuracy_threshold = 1e-6;

    std:: cout << "Evaluating..." << std::endl;
 
    std::vector<Statistics> statistics(n_episodes);

    DiscreteMDPCounts model(environment->getNStates(),
							environment->getNActions(),
							1.0 / (real) environment->getNStates());
	
	for (int episode = 0; episode < n_episodes; ++episode) {
		std::cerr << "Episode: " << episode << std::endl;
		statistics[episode].total_reward = 0.0;
		statistics[episode].discounted_reward = 0.0;
		statistics[episode].steps = 0;
		real discount = 1.0;
		environment->Reset();
		algorithm->Reset();
		bool action_ok = true;
		int prev_state = - 1;
		int action = -1;
		for (int t=0; t < n_steps; ++t) {
			printf ("%d\n", t);
			int state = environment->getState();
			real reward = environment->getReward();
			if (t) {
				model.AddTransition(prev_state, action, reward, state);
			}
			prev_state = state;
			statistics[episode].total_reward += reward;
			statistics[episode].discounted_reward += discount * reward;
			discount *= gamma;
			statistics[episode].steps = t;
			action = algorithm->Act(reward, state);
			if (!action_ok) {
				break;
			}		
			 action_ok = environment->Act(action);
		}

		printf ("# Mean model\n");
		const DiscreteMDP* mean_mdp = model.CreateMDP();
		ValueIteration value_iteration(mean_mdp, gamma);
		value_iteration.ComputeStateActionValues(accuracy_threshold);
		
		std::vector<const DiscreteMDP*> mdp_samples;
		std::vector<DiscretePolicy*> policy_samples;
		// collect some statistics
		printf ("# Samples\n");
		for (int n_samples=1; n_samples<=8; ++n_samples) {
			mdp_samples.push_back(model.generate());
			printf ("# Optimistic policy\n");
			ValueIteration vi(mdp_samples[n_samples - 1], gamma);
			vi.ComputeStateActionValues(accuracy_threshold);

			Vector w(n_samples);
			real w_i = 1.0 / (real) n_samples;
			for (int i=0; i < n_samples; ++i) {
				w(i) = w_i;
			}
			
			printf ("# Multi-MDP %d samples\n", n_samples);
			MultiMDPValueIteration mmvi(w, mdp_samples, gamma);
			mmvi.ComputeStateActionValues(accuracy_threshold);
			
			
		}
		delete mean_mdp;
	}
    return statistics;
}

#endif
