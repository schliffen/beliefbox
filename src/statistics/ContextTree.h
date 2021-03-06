/* -*- Mode: c++;  -*- */
// copyright (c) 2010 by Christos Dimitrakakis <christos.dimitrakakis@gmail.com>
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTEXT_TREE_H
#define CONTEXT_TREE_H

#include <vector>
#include "real.h"
#include "Vector.h"
#include "Ring.h"


/** An Bayesian variable order Markov model implemented as a context tree.

    This is a dynamically-updated model, usable online. From the paper:
    "Bayesian Variable Order Markov Models",
    C. Dimitrakakis, AI-STATS 2010.
*/
class ContextTree
{
public:
	// public classes
	struct Node
	{
        int N_obs; ///< total number of observations
        int n_branches; ///< The number of symbols we condition on
        int n_outcomes; ///< The number of symbols to predict
        int depth; ///< depth
        Node* prev; ///< previous node
        std::vector<Node*> next; ///< pointers to next nodes
		Vector P; ///< probability of next symbols
		Vector alpha; ///< parameters of next symbols
        const real prior_alpha; ///< implicit prior value of alpha
        real w; ///< backoff weight
        real log_w; ///< log of w
        real log_w_prior; ///< initial value
		Node(int n_branches_,
			 int n_outcomes_);
		Node(Node* prev_);
		~Node();
		real Observe(Ring<int>& history,
					 Ring<int>::iterator x,
					 int y,
					 real probability);
		void Show();
		int NChildren();	
	};
	
	// public methods
	ContextTree(int n_branches_, int n_symbols_, int max_depth_= 0);
	~ContextTree();
	real Observe(int x, int y);
	void Show();
	int NChildren();
protected: 
	int n_branches;
	int n_symbols;
	int max_depth;
	Node* root;
    Ring<int> history;
};



#endif
