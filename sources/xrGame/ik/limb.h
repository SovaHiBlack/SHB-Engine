/*
  This source code is a part of IKAN.
  Copyright (c) 2000 University of Pennsylvania
  Center for Human Modeling and Simulation
  All Rights Reserved.

  IN NO EVENT SHALL THE UNIVERSITY OF PENNSYLVANIA BE LIABLE TO ANY
  PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
  DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS
  SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF PENNSYLVANIA
  HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

  Permission to use, copy, modify and distribute this software and its
  documentation for educational, research and non-profit purposes,
  without fee, and without a written agreement is hereby granted,
  provided that the above copyright notice and the following three
  paragraphs appear in all copies. For for-profit purposes, please
  contact University of Pennsylvania
 (http://hms.upenn.edu/software/ik/ik.html) for the software license
  agreement.


  THE UNIVERSITY OF PENNSYLVANIA SPECIFICALLY DISCLAIM ANY
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
  BASIS, AND THE UNIVERSITY OF PENNSYLVANIA HAS NO OBLIGATION
  TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
  MODIFICATIONS.

 */


#ifndef _LIMBH
#define _LIMBH

#include "aint.h"
#include "Dof7control.h"
#include "eulersolver.h"



class Limb 
{
private:
	//    short check_limits;
	SRS   solver;

	short check_limits;
	short solve;
	short num_singular;
	short euler1, euler2;

	f32 singular_pts[4];
	f32 x3;

	// Set by SetGoal or SetGoalPos
	// For SetGoal:
	// 
	// PSI[0] = valid psi solns for (family1,family1) of R1 and R2
	// PSI[1] = valid psi solns for (family1,family2) of R1 and R2
	// PSI[2] = valid psi solns for (family2,family1) of R1 and R2
	// PSI[3] = valid psi solns for (family2,family2) of R1 and R2
	// 
	// For SetGoalPos:
	// PSI[0] = valid psi solns for family1 of R1 
	// PSI[1] = valid psi solns for family2 of R1 

	AngleIntList PSI[4]; 

	// The euler convention fors the first and second S joints 
public:
	AngleInt jt_limits[7];
private:
	f32 min[7], max[7];

	s32 check_r_joint(f32& val);
	
	void	extract_s1(const Matrix R1, f32 s1[3]);
	void	extract_s1s2(const Matrix R1, const Matrix R2, f32 s1[3], f32 s2[3]);

	void	extract_s1_family(const Matrix R1, s32 family, f32 s1[3]);
	void	extract_s1s2_family(const Matrix R1, const Matrix R2, 
								s32 family1, s32 family2,
								f32 s1[3], f32 s2[3]);
	s32		set_goal(const Matrix G);
	s32		set_goal_pos(const f32 g[3], const Matrix E);

	void	solve_aux(f32 swivel_angle, f32 x[]);
	void	solve_aux_family(s32 family_set, f32 swivel_angle, f32 x[]);

	void	solve_pos_aux(f32 swivel_angle, f32 x[]);
	void	solve_pos_aux_family(s32 family, f32 swivel_angle, f32 x[]);

	s32		try_swivel_angle(s32 solve, f32 swivel_angle, f32 x[]);
	s32		try_singularities(s32 solve, f32& swivel_angle, f32 x[]);
	s32		try_closeby_singularity(s32 solve, f32& swivel_angle, f32 x[]);

public:
	void	get_R1R2psi(AngleIntList psi[]);
	void	get_R1psi(AngleIntList psi[]);

	Limb() {}

	void init(const Matrix T, 
		  const Matrix S,
			  s32 s1_euler,
			  s32 s2_euler,
		  const f32 proj_axis[3],
		  const f32 pos_axis[3],
		  const f32 min[7],
		  const f32 max[7]);

	Limb(const Matrix T, 
	 const Matrix S,
		 s32 s1_euler,
		 s32 s2_euler,
	 const f32 proj_axis[3],
	 const f32 pos_axis[3],
	 const f32 Min[7],
	 const f32 Max[7])
	{
	init(T,S,s1_euler,s2_euler,proj_axis,pos_axis,Min,Max);
	}
	
	~Limb() {}

	void SetTMatrix(const Matrix TT)
	{
		solver.SetTMatrix(TT);
	}
	void SetSMatrix(const Matrix SS)
	{
		solver.SetSMatrix(SS);
	}
	s32 SetGoalPos(const f32 g[3], const Matrix E, s32 limits_on);
	s32 SetGoal(const Matrix G, s32 limits_on);

	s32 SetAimGoal(const f32 goal[3], const f32 axis[3], f32 flex_angle)
	{
		solver.SetAimGoal(goal, axis, flex_angle);
	}

	s32 SolveAim(f32 x[3], f32 psi_angle);

	f32 PosToAngle(const f32 p[3]);
	f32 KneeAngle( const f32 goal_pos[3], const f32 knee_pos[3] );

	s32 Solve(f32 x[7], f32* new_psi = 0, f32* new_pos = 0);

	s32 SolveByAngle(f32 psi,
					 f32 x[7],
					 f32* new_psi = 0,
					 f32* new_pos= 0);

	s32 SolveByPos(const f32 pos[3],
				   f32 x[7],
				   f32* new_psi = 0,
				   f32* new_pos = 0);

	s32 InLimits(const f32 x[7]) const;
	void Debug(pstr file1, pstr file2);

	// Must call SetGoal first with joint limits turned on
	void GetPsiIntervals(AngleIntList &f11,
			 AngleIntList &f12,
			 AngleIntList &f21,
			 AngleIntList &f22)
	{
	PSI[0].Copy(f11);
	PSI[1].Copy(f12);
	PSI[2].Copy(f21);
	PSI[3].Copy(f22);
	}

	// Must call SetGoalPos first with joint limits turned on
	void GetPsiPosIntervals(AngleIntList &f1,
				AngleIntList &f2)
	{
	PSI[0].Copy(f1);
	PSI[1].Copy(f2);
	}

	void ForwardKinematics(f32 x[7], Matrix R);

	s32 GetJointIntervals(Matrix G, AngleIntList f1[6], AngleIntList f2[6]);
};

#endif
