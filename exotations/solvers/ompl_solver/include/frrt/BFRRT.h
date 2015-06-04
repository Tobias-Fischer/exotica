/*
 * BFRRT.h
 *
 *  Created on: 1 Jun 2015
 *      Author: yiming
 */

#ifndef EXOTICA_EXOTATIONS_SOLVERS_OMPL_SOLVER_INCLUDE_FRRT_BFRRT_H_
#define EXOTICA_EXOTATIONS_SOLVERS_OMPL_SOLVER_INCLUDE_FRRT_BFRRT_H_

#include <ompl/geometric/planners/PlannerIncludes.h>
#include <ompl/base/spaces/RealVectorStateSpace.h>
#include <ompl/base/goals/GoalState.h>
#include <ompl/datastructures/NearestNeighbors.h>
#include <ompl/tools/config/SelfConfig.h>
#include "exotica/EXOTica.hpp"
#include <ompl_solver/OMPLGoalSampler.h>
#include <ik_solver/ik_solver.h>

namespace ompl
{
	namespace geometric
	{
		///	\brief	The bi-directional FRRT
		class BFRRT: public base::Planner
		{
			public:
				/*
				 * \brief	Constructor
				 * @param	si	Space information
				 */
				BFRRT(const base::SpaceInformationPtr &si);

				/*
				 * \brief	Destructor
				 */
				virtual ~BFRRT();

				virtual void getPlannerData(base::PlannerData &data) const;
				virtual base::PlannerStatus solve(const base::PlannerTerminationCondition &ptc);
				virtual void clear();

				/** \brief Set the range the planner is supposed to use.
				 This parameter greatly influences the runtime of the
				 algorithm. It represents the maximum length of a
				 motion to be added in the tree of motions. */
				void setRange(double distance)
				{
					maxDistance_ = distance;
				}

				/** \brief Get the range the planner is using */
				double getRange() const
				{
					return maxDistance_;
				}

				/** \brief Set a different nearest neighbors datastructure */
				template<template<typename T> class NN>
				void setNearestNeighbors()
				{
					tStart_.reset(new NN<Motion*>());
					tGoal_.reset(new NN<Motion*>());
				}

				virtual void setup();

				/*
				 * \brief	Set up the local planner (EXOTica)
				 * @param	xml_file	XML configuration file
				 * @param	scene		EXOTica scene
				 * @return	True if succeeded, false otherwise
				 */
				bool setUpLocalPlanner(const std::string & xml_file,
						const exotica::Scene_ptr & scene);

				bool resetSceneAndGoal(const exotica::Scene_ptr & scene,
						const Eigen::VectorXd & goal);
			protected:

				/** \brief Representation of a motion */
				class Motion
				{
					public:

						Motion() :
										root(NULL),
										state(NULL),
										inter_state(NULL),
										parent(NULL),
										global_valid_(true)
						{
						}

						Motion(const base::SpaceInformationPtr &si) :
										root(NULL),
										state(si->allocState()),
										inter_state(NULL),
										parent(NULL),
										global_valid_(true)
						{
						}

						~Motion()
						{
						}

						bool isChecked(const Motion *motion)
						{
							return checked_.find(motion) != checked_.end() ? true : false;
						}
						const base::State *root;
						base::State *state;
						///	Internal state
						base::State *inter_state;
						///	The parent node
						Motion *parent;
						///	The internal flexible path
						boost::shared_ptr<Eigen::MatrixXd> internal_path;
						bool global_valid_;
					private:
						std::map<const Motion*, bool> checked_;

				};

				/** \brief A nearest-neighbor datastructure representing a tree of motions */
				typedef boost::shared_ptr<NearestNeighbors<Motion*> > TreeData;

				/** \brief Information attached to growing a tree of motions (used internally) */
				struct TreeGrowingInfo
				{
						Motion *xmotion;
						bool start;
						Motion *last_s;
						Motion *last_g;
				};

				/** \brief The state of the tree after an attempt to extend it */
				enum GrowState
				{
					/// no progress has been made
					TRAPPED,
					/// progress has been made towards the randomly sampled state
					ADVANCED,
					/// the randomly sampled state was reached
					REACHED
				};

				/** \brief Free the memory allocated by this planner */
				void freeMemory();

				/** \brief Compute distance between motions (actually distance between contained states) */
				double distanceFunction(const Motion *a, const Motion *b) const
				{
					return si_->distance(a->state, b->state);
				}

				/** \brief Grow a tree towards a random state */
				GrowState growTree(TreeData &tree, TreeGrowingInfo &tgi, Motion *rmotion);

				/** \brief State sampler */
				base::StateSamplerPtr sampler_;

				/** \brief The start tree */
				TreeData tStart_;

				/** \brief The goal tree */
				TreeData tGoal_;

				/** \brief The maximum length of a motion to be added to a tree */
				double maxDistance_;

				/** \brief The random number generator */
				RNG rng_;

				/** \brief The pair of states in each tree connected during planning.  Used for PlannerData computation */
				std::pair<base::State*, base::State*> connectionPoint_;

			private:
				///	Local solver
				bool localSolve(Motion *sm, base::State *is, Motion *gm);
				exotica::Server_ptr ser_;
				exotica::IKsolver_ptr local_solver_;

				exotica::TaskSqrError_ptr local_task_;
				boost::shared_ptr<exotica::Identity> local_map_;
				exotica::TaskSqrError_ptr collision_task_;
				Eigen::VectorXd global_goal_;
		};
	}	//	Geometric namespace
}	//	OMPL namespace

#endif /* EXOTICA_EXOTATIONS_SOLVERS_OMPL_SOLVER_INCLUDE_FRRT_BFRRT_H_ */
