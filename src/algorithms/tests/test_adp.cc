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

#include "MountainCar.h"
#include "CartPole.h"
#include "Pendulum.h"
#include "MersenneTwister.h"
#include "DiscreteChain.h"
#include "EasyClock.h"
#include "BasisSet.h"
#include "RepresentativeStateValueIteration.h"
#include "debug.h"
#include <getopt.h>

struct Options {
    real gamma; = 0.95;
    int grid_size; = 4;
    real grid_scale; = 0.25;
    int n_samples = 1;
    real threshold = 0;
    int n_iterations = 1000;
    std::string algorithm_name = "RSVI";
    std::string environment_name = "Pendulum";
};

int main (int argc, char* argv[])
{
    MersenneTwisterRNG rng;
    Options options;
    std::string algorithm_name;
    std::string environment_name;
    {
        //options
        int c;
        while(1){
            int option_index = 0;
            static struct option long_options[] = {
                {"help", no_argument, 0, 0}, //0
                {"n_iterations",required_argument, 0, 0}, //1
                {"gamma", required_argument, 0, 0}, //2
                {"n_samples",required_argument, 0 ,0}, //3
                {"environment", required_argument, 0, 0}, //4
                {"grid", required_argument, 0, 0}, //5
                {"scale", required_argument, 0, 0}, //6
                {"algorithm", required_argument, 0, 0}, //7
                {"threshold", required_argument, 0, 0}, //8
                {0, 0, 0, 0}
            };
            c = getopt_long(argc, argv, "", long_options, &option_index);
            if(c == -1)
                break;
			
            switch (c){
            case 0:
#if 0
                printf ("option %s (%d)", long_options[option_index].name, option_index);
                if (optarg)
                    printf (" with arg %s", optarg);
                printf ("\n");
#endif	
                switch (option_index) {
                case 1: options.n_iterations = atoi(optarg); break;
                case 2: options.gamma = atof(optarg); break;
                case 3: options.n_samples = atoi(optarg); break;
                case 4: options.environment_name = optarg; break;
                case 5: options.grid_size = atoi(optarg); break;
                case 6: options.grid_scale = atof(optarg); break;
                case 7: options.algorithm_name = optarg; break;
                case 8: options.threshold = atof(optarg); break;
                default:
                    printf ("Usage options\n");
                    for (int i=0; long_options[i].name; i++) {
                        printf("--%s", long_options[i].name);
                        switch (long_options[i].has_arg) {
                        case no_argument: printf ("\n"); break;
                        case optional_argument: printf (" [arg]\n"); break;
                        case required_argument: printf (" arg\n"); break;
                        }
                    }
                    exit(0);
                    break;
                }
                break;
            default:
                printf ("Usage options\n");
                for (int i=0; long_options[i].name; i++) {
                    printf("--%s", long_options[i].name);
                    switch (long_options[i].has_arg) {
                    case no_argument: printf ("\n"); break;
                    case optional_argument: printf (" [arg]\n"); break;
                    case required_argument: printf (" arg\n"); break;
                    }
                }
                exit(-1);
            }
        }
        if (optind < argc) {
            printf ("non-option ARGV-elements: ");
            while (optind < argc) {
                printf ("%s ", argv[optind++]);
                
            }
            printf ("\n");
        }		
    }


    
    
    Environment<Vector, int>* environment_ptr  = NULL;
    if (options.environment_name == "MountainCar") {
        environment_ptr = new MountainCar;
    } else if (options.environment_name == "Pendulum") {
        environment_ptr = new Pendulum;
    } else if (options.environment_name == "CartPole") {
        environment_ptr = new CartPole;
    } else {
        std::cerr << "Unknown environment: " <<  environment_name << std::endl;
        exit(-1);
    }
    Environment<Vector, int>& environment = *environment_ptr;
                                          
    logmsg("Generating grid\n");
    EvenGrid grid(environment.StateLowerBound(),
                  environment.StateUpperBound(),
                  options.grid_size);

    logmsg("Creating kernel\n");
    // use an RBF basis for the kernel fromthe grid
    RBFBasisSet kernel(options.grid, options.grid_scale);

    logmsg("Selecting representative states\n");
    // create the set of representative states (identical to the grid)
    std::vector<Vector> states;
    std::vector<int> actions;
    for (int i=0; i<grid.getNIntervals(); ++i) {
        states.push_back(grid.getCenter(i));
    }
    // just use all the actions since it's a discrete space
    for (int i=0; i<environment.getNActions(); ++i) {
        actions.push_back(i);
    }
    
    if (algorithm_name == "RSVI") {
        logmsg("setting up RSVI\n");
        RepresentativeStateValueIteration<Vector, int, RBFBasisSet, Environment<Vector, int> > rsvi(options.gamma, kernel, states, actions, environment, options.n_samples);
        
        logmsg("Calculating approximate value function\n");
        rsvi.CalculateValues(threshold, n_iterations);
	FILE* outfile = fopen(stdout, "%s_%s_%d_%f_%d_%d\n",
                              options.algorithm_name,
                              options.environment_name,
                              options.grid_size,
                              options.grid_scale,
                              options.n_samples,
                              options.n_iterations);
        
        for (int i=0; i<grid.getNIntervals(); ++i) {
            
            printf("%f value ", rsvi.getValue(states.at(i)));
        }
        printf ("# value\n");
    }

    printf("\nDone\n");
        delete envr
    return 0.0;
}


#endif