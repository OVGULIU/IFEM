// $Id: LinearElasticity.h,v 1.21 2011-02-08 09:06:02 kmo Exp $
//==============================================================================
//!
//! \file LinearElasticity.h
//!
//! \date Nov 12 2009
//!
//! \author Knut Morten Okstad / SINTEF
//!
//! \brief Integrand implementations for linear elasticity problems.
//!
//==============================================================================

#ifndef _LINEAR_ELASTICITY_H
#define _LINEAR_ELASTICITY_H

#include "Elasticity.h"


/*!
  \brief Class representing the integrand of the linear elasticity problem.
*/

class LinearElasticity : public Elasticity
{
public:
  //! \brief Default constructor.
  //! \param[in] n Number of spatial dimensions
  LinearElasticity(unsigned short int n = 3);
  //! \brief Empty destructor.
  virtual ~LinearElasticity() {}

  //! \brief Evaluates the integrand at an interior point.
  //! \param elmInt The local integral object to receive the contributions
  //! \param[in] detJW Jacobian determinant times integration point weight
  //! \param[in] N Basis function values
  //! \param[in] dNdX Basis function gradients
  //! \param[in] X Cartesian coordinates of current integration point
  virtual bool evalInt(LocalIntegral*& elmInt, double detJW,
		       const Vector& N, const Matrix& dNdX,
		       const Vec3& X) const;

  //! \brief Evaluates the integrand at a boundary point.
  //! \param elmInt The local integral object to receive the contributions
  //! \param[in] detJW Jacobian determinant times integration point weight
  //! \param[in] N Basis function values
  //! \param[in] dNdX Basis function gradients
  //! \param[in] X Cartesian coordinates of current integration point
  //! \param[in] normal Boundary normal vector at current integration point
  virtual bool evalBou(LocalIntegral*& elmInt, double detJW,
		       const Vector& N, const Matrix& dNdX,
		       const Vec3& X, const Vec3& normal) const;

private:
  // Work array declared as member to avoid frequent re-allocation
  // within the numerical integration loop (for reduced overhead)
  mutable Matrix CB; //!< Result of the matrix-matrix product C*B
};

#endif
