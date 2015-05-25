/*
 * OMPLGoalSampler.h
 *
 *  Created on: 5 Aug 2014
 *      Author: s0972326
 */

#ifndef OMPLGOALSAMPLER_H_
#define OMPLGOALSAMPLER_H_

#include <ompl/base/goals/GoalSampleableRegion.h>
#include <ompl/base/goals/GoalRegion.h>
#include "ompl_solver/OMPLsolver.h"
#include "ompl_solver/OMPLProblem.h"
#include "ompl_solver/OMPLGoal.h"

namespace exotica
{

    class OMPLGoal;

	class OMPLsolver;

    class OMPLGoalSampler : public OMPLGoal, public ompl::base::GoalSampleableRegion
	{
		public:
            OMPLGoalSampler (const ompl::base::SpaceInformationPtr &si, OMPLProblem_ptr prob);
			virtual
			~OMPLGoalSampler ();
            virtual void sampleGoal(ompl::base::State *st) const ;
            virtual unsigned int maxSampleCount() const ;
            virtual double distanceGoal(const ompl::base::State *st) const;

        private:

			OMPLProblem_ptr prob_;
			boost::shared_ptr<OMPLsolver> sol_;
			ompl::base::StateSamplerPtr                      default_sampler_;
            ompl::base::State *goalState_;

	};
	typedef boost::shared_ptr<exotica::OMPLGoalSampler> OMPLGoalSampler_ptr;
} /* namespace exotica */

#endif /* OMPLGOALSAMPLER_H_ */
