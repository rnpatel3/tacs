#ifndef TACS_MITC9_H
#define TACS_MITC9_H

/*
  Copyright (c) 2015-2016 Graeme Kennedy. All rights reserved. 
*/

#include "TACSElement.h"
#include "FSDTStiffness.h"
#include "TACSGibbsVector.h"

/*
  A Mixed-Interpolation of Tensorial Components element for dynamic
  analysis.

  The following class implements a geometrically nonlinear
  finite-element shell for large displacement/rotation problems.  The
  element permits arbitrary rotation/displacement rigid body motion.
  The rotational parametrization is based on the quaternions with an
  added constraint at each node.

  The shell formulation utilizes through-thickness strain/kinematic
  assumptions that are based on first-order shear deformation
  theory. The theory takes into account the nonlinear rotational
  kinematics that are required to obtain strain-free rotation of the
  elements.

  The drilling degree of freedom is handled through the use of a penalty
  term that penalizes the discrepancy between the in-plane rotations
  predicted from nonlinear shell theory and those predicted by stress-
*/
class MITC9 : public TACSElement {
 public:
  static const int ORDER = 3;
  static const int NUM_NODES = ORDER*ORDER;
  static const int NUM_DISPS = 8;
  static const int NUM_STRESSES = 8;
  static const int NUM_EXTRAS = 4;
  
  MITC9( FSDTStiffness *_stiff, 
         TACSGibbsVector *_gravity=NULL,
         TACSGibbsVector *_vInit=NULL, 
         TACSGibbsVector *_omegaInit=NULL );
  ~MITC9();

  // Return the sizes of the array components
  // ----------------------------------------
  int numDisplacements();
  int numStresses();
  int numExtras();
  int numNodes();
  
  // Functions to determine the variable names and quantities
  // --------------------------------------------------------
  const char * elementName();
  const char * displacementName( int i );
  const char * stressName( int i );
  const char * strainName( int i );
  const char * extraName( int i );
  
  ElementType getElementType();

  // Functions for handling the design variables
  // -------------------------------------------
  void setDesignVars( const TacsScalar dvs[], int numDVs );
  void getDesignVars( TacsScalar dvs[], int numDVs );
  void getDesignVarRange( TacsScalar lowerBound[], 
			  TacsScalar upperBound[], int numDVs );

  // Retrieve the initial values of the state variables
  // --------------------------------------------------
  void getInitCondition( TacsScalar vars[],
			 TacsScalar dvars[],
			 const TacsScalar X[] );

  // Compute the kinetic and potential energies
  // ------------------------------------------
  void computeEnergies( double time,
                        TacsScalar *_Te, TacsScalar *_Pe,
			const TacsScalar X[],
			const TacsScalar vars[],
			const TacsScalar dvars[] );

  // Compute the residual of the governing equations
  // -----------------------------------------------
  void addResidual( double time, 
                    TacsScalar res[],
		    const TacsScalar X[],
		    const TacsScalar vars[],
		    const TacsScalar dvars[],
		    const TacsScalar ddvars[] );

  // Compute the Jacobian of the governing equations
  // -----------------------------------------------
  void addJacobian( double time, TacsScalar J[],
		    double alpha, double beta, double gamma,
		    const TacsScalar X[],
		    const TacsScalar vars[],
		    const TacsScalar dvars[],
		    const TacsScalar ddvars[] );

  // Add the product of the adjoint with the derivative of the design variables
  // --------------------------------------------------------------------------
  void addAdjResProduct( double time, double scale,
			 TacsScalar dvSens[], int dvLen,
			 const TacsScalar psi[],
			 const TacsScalar Xpts[],
			 const TacsScalar vars[],
			 const TacsScalar dvars[],
			 const TacsScalar ddvars[] );

  // Member functions for evaluating global functions of interest
  // ------------------------------------------------------------
  TACSConstitutive * getConstitutive();
  
  // Get the number of Gauss quadrature points
  // -----------------------------------------
  int getNumGaussPts();
  double getGaussWtsPts( const int num, double pt[] );

  // Get the shape functions from the element
  // ----------------------------------------
  void getShapeFunctions( const double pt[], double N[] );
  
  // Return the determinant of the Jacobian of the transformation
  // ------------------------------------------------------------
  TacsScalar getDetJacobian( const double * pt, 
                             const TacsScalar Xpts[] );

  // Get the strain and the parametric location from the element
  // -----------------------------------------------------------
  void getStrain( TacsScalar e[],
                  const double pt[],
                  const TacsScalar X[],
                  const TacsScalar vars[] );

  // This function adds the sensitivity of the strain to the state variables
  // -----------------------------------------------------------------------
  void addStrainSVSens( TacsScalar sens[],
			const double pt[], 
			const TacsScalar scale,
			const TacsScalar esens[], 
			const TacsScalar Xpts[],
			const TacsScalar vars[] );

  // Functions for post-processing
  // -----------------------------
  void addOutputCount( int * nelems, int * nnodes, int * ncsr );
  void getOutputData( unsigned int out_type, 
		      double * data, int ld_data,
		      const TacsScalar Xpts[],
		      const TacsScalar vars[] );
  void getOutputConnectivity( int * con, int node );

  // Test the strain implementation
  // ------------------------------
  void testStrain( const TacsScalar X[] );
  
 private:
  // Helper functions required for analysis
  void computeAngularVelocity( TacsScalar omega[],
			       const TacsScalar vars[],
			       const TacsScalar dvars[] );

  // Compute the angular acceleration at the nodes
  void computeAngularAccel( TacsScalar domega[],
			    const TacsScalar vars[],
			    const TacsScalar dvars[],
			    const TacsScalar ddvars[] );

  // Compute the reference frames at each node of the element
  void computeFrames( TacsScalar Xr[], const TacsScalar X[] );

  // Compute the directors for each node
  void computeDirectors( TacsScalar d[],
			 const TacsScalar vars[],
			 const TacsScalar Xr[] );

  // Compute the derivative of the directors w.r.t. the variables
  void computeDirectorDeriv( TacsScalar dirdq[],
			     const TacsScalar vars[],
			     const TacsScalar Xr[] );

  // Evaluate the strain
  void evalStrain( TacsScalar e[],
		   const TacsScalar Ur[], 
		   const TacsScalar dr[],
		   const TacsScalar Xdinv[],
		   const TacsScalar zXdinv[],
		   const TacsScalar T[] );

  // Evaluate the derivative of the strain w.r.t. the element variables 
  void evalBmat( TacsScalar e[], TacsScalar B[],
		 const double N[], const double Na[], const double Nb[],
		 const TacsScalar Ur[], const TacsScalar dr[],
		 const TacsScalar Xdinv[], const TacsScalar zXdinv[],
		 const TacsScalar T[], const TacsScalar dirdq[] );

  // Add the interpolated strain to the strain vector
  void addTyingStrain( TacsScalar e[],
		       const double N13[], const double N23[],
		       const TacsScalar g13[], const TacsScalar g23[],
		       const TacsScalar Xdinv[], const TacsScalar T[] );

  // Add the contribution from the tying strain to the b-matrix
  void addTyingBmat( TacsScalar B[],
		     const double N13[], const double N23[],
		     const TacsScalar b13[], const TacsScalar b23[],
		     const TacsScalar Xdinv[], const TacsScalar T[] );

  // Compute the shear strain at the tying points
  void computeTyingStrain( TacsScalar g13[], TacsScalar g23[],
			   const TacsScalar X[], const TacsScalar Xr[],
			   const TacsScalar vars[], 
			   const TacsScalar dir[] );

  // Compute the derivative of the strain at the tying points
  void computeTyingBmat( TacsScalar g13[], TacsScalar g23[],
			 TacsScalar b13[], TacsScalar b23[],
			 const TacsScalar X[], const TacsScalar Xr[],
			 const TacsScalar vars[], const TacsScalar dir[],
			 const TacsScalar dirdq[] );

  // Add the terms from the geometric stiffness matrix
  void addGmat( TacsScalar J[], 
		const TacsScalar scale, const TacsScalar s[],
		const double N[], const double Na[], const double Nb[],
		const TacsScalar Ur[], const TacsScalar dr[],
		const TacsScalar Xdinv[], const TacsScalar zXdinv[],
		const TacsScalar T[], const TacsScalar Xr[],
		const TacsScalar dirdq[] );

  // Add the term from the drilling rotation
  TacsScalar computeRotPenalty( const double N[],
				const TacsScalar Xa[],
				const TacsScalar Xb[],
				const TacsScalar Ua[],
				const TacsScalar Ub[],
				const TacsScalar vars[] );

  // Compute the derivative of the rotation term 
  TacsScalar computeBRotPenalty( TacsScalar brot[], const double N[], 
				 const double Na[], const double Nb[],
				 const TacsScalar Xa[], 
				 const TacsScalar Xb[],
				 const TacsScalar Ua[], 
				 const TacsScalar Ub[],
				 const TacsScalar vars[] );

  // Add the geometric stiffness term from the rotation
  void addGRotMat( TacsScalar J[], const TacsScalar scale, 
		   const double N[], const double Na[], const double Nb[],
		   const TacsScalar Xa[], const TacsScalar Xb[],
		   const TacsScalar Ua[], const TacsScalar Ub[],
		   const TacsScalar vars[] );

  // Add the geometric stiffness matrix from the tying strain
  void addTyingGmat( TacsScalar J[],
		     const TacsScalar w13[], const TacsScalar w23[],
		     const TacsScalar X[], const TacsScalar Xr[],
		     const TacsScalar vars[], const TacsScalar dir[],
		     const TacsScalar dirdq[] );

  // Add to the weights required to compute the geometric stiffness
  void addTyingGmatWeights( TacsScalar w13[], TacsScalar w23[],
			    const TacsScalar scalar,
			    const TacsScalar s[],
			    const double N13[], const double N23[],
			    const TacsScalar Xdinv[],
			    const TacsScalar T[] );

  // Compute the product of the stress and the strain
  inline TacsScalar strainProduct( const TacsScalar s[], 
				   const TacsScalar e[] ){
    return (e[0]*s[0] + e[1]*s[1] + e[2]*s[2] +
	    e[3]*s[3] + e[4]*s[4] + e[5]*s[5] + 
	    e[6]*s[6] + e[7]*s[7]);
  }

  // Set the pointers to quadrature points/weights
  const double *gaussPts, *gaussWts;

  // The stiffness object
  FSDTStiffness *stiff;

  // The gravity vector (if any)
  TACSGibbsVector *gravity;

  // Initial velocity/angular velocity
  TACSGibbsVector *vInit, *omegaInit;

  // The names of the displacements, stresses etc.
  static const char * elemName;
  static const char * dispNames[NUM_DISPS];
  static const char * stressNames[NUM_STRESSES];
  static const char * strainNames[NUM_STRESSES];
  static const char * extraNames[NUM_EXTRAS];
};
#endif // TACS_MITC9_H
