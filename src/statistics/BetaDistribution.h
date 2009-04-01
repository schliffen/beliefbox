/* -*- Mode: C++; -*- */
/* VER: $Id: Distribution.h,v 1.3 2006/11/06 15:48:53 cdimitrakakis Exp cdimitrakakis $*/
// copyright (c) 2004-2006 by Christos Dimitrakakis <christos.dimitrakakis@gmail.com>
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BETA_DISTRIBUTION_H
#define BETA_DISTRIBUTION_H

#include "Distribution.h"

class BetaDistribution : public ConjugatePrior
{
public:
    real alpha; ///< number of ones obsered
    real beta; ///< number of zeros observed

    /// Beta distribution default constructor: alpha,beta=1, the
    /// uniform distribution
    BetaDistribution()
    {
        alpha = 1.0f;
        beta = 1.0f;
    }
    /// Give your own alpha/beta values
    BetaDistribution(real alpha, real beta)
    {
        this->alpha = alpha;
        this->beta = beta;
    }

    virtual real pdf(real x) const;
    real log_pdf(real x) const;
    virtual void calculatePosterior(real x);
    virtual void setMean(real mean);
    virtual void setVariance(real var);
    virtual real getMean() const;
    virtual real getVariance();
    virtual real generate();
};


#endif
