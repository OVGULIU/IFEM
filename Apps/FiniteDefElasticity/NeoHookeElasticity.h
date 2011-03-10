// $Id$
//==============================================================================
//!
//! \file NeoHookeElasticity.h
//!
//! \date Jul 1 2010
//!
//! \author Knut Morten Okstad / SINTEF
//!
//! \brief Integrand implementations for Neo-Hooke elasticity problems.
//!
//==============================================================================

#ifndef _NEO_HOOKE_ELASTICITY_H
#define _NEO_HOOKE_ELASTICITY_H

#include "NonlinearElasticity.h"


/*!
  \brief Class representing the integrand of nonlinear hyperelastic problems.
  \details This class implements the Neo-Hooke hyperelastic material behavior.
*/

class NeoHookeElasticity : public NonlinearElasticity
{
public:
  //! \brief The default constructor invokes the parent class constructor only.
  //! \param[in] n Number of spatial dimensions
  NeoHookeElasticity(unsigned short int n = 3)
  : NonlinearElasticity(n), C(n), Ci(n) { fullCmat = true; }

  //! \brief Empty destructor.
  virtual ~NeoHookeElasticity() {}

  //! \brief Prints out problem definition to the given output stream.
  virtual void print(std::ostream& os) const;

  //! \brief Defines material properties for current volume patch.
  //! \param[in] Emod    Young's modulus
  //! \param[in] Poiss   Poisson's ratio
  //! \param[in] Density Mass density
  virtual void setMaterial(double Emod, double Poiss, double Density);

protected:
  //! \brief Calculates some kinematic quantities at current point.
  //! \param[in] dNdX Basis function gradients at current point
  //! \param[out] _F Deformation gradient at current point
  //! \param[out] _E Green-Lagrange strain tensor at current point
  //!
  //! \details The deformation gradient \b F and the Green-Lagrange tensor \b E
  //! are established. In addition, the right Cauchy-Green tensor \b C and its
  //! inverse \b Ci are formed.
  //! All quantities are stored internally in mutable data members.
  virtual bool kinematics(const Matrix& dNdX, Tensor& _F, SymmTensor& _E) const;

  //! \brief Forms tangential tensorial quantities needed by the evalInt method.
  //! \param[in] X Cartesian coordinates of current integration point
  //! \param[out] Ctan Tangential constitutive tensor at current point
  //! \param[out] S 2nd Piola-Kirchhoff stress tensor at current point
  virtual bool formTangent(Matrix& Ctan, SymmTensor& S, const Vec3& X) const;

  //! \brief Forms the 2nd Piola-Kirchhoff stress tensor.
  //! \param[in] dNdX Basis function gradients at current integration point
  //! \param[in] X Cartesian coordinates of current integration point
  //! \param[out] S 2nd Piola-Kirchhoff stress tensor at current point
  virtual bool formStressTensor(const Matrix& dNdX, const Vec3& X,
				SymmTensor& S) const;

  //! \brief Forms the 2nd Piola-Kirchhoff stress tensor.
  virtual void formStressTensor(SymmTensor& S) const;

public:
  //! \brief Sets up the tangential constitutive matrix at current point.
  //! \param[out] Ctan \f$6\times6\f$-matrix (in 3D) or \f$3\times3\f$-matrix
  //! (in 2D), representing the tangential constitutive tensor
  //! \param[in] X Cartesian coordinates of current point
  virtual bool formCmatrix(Matrix& Ctan, const Vec3& X, bool = false) const;

protected:
  double lambda; //!< First Lame parameter
  double mu;     //!< Second Lame parameter (shear modulus)
  double kappa;  //!< Bulk modulus
  mutable double     J;  //!< Determinant of the deformation gradient
  mutable SymmTensor C;  //!< Right Cauchy-Green deformation tensor
  mutable SymmTensor Ci; //!< Inverse of the right Cauchy-Green tensor
};


/*!
  \brief Neo-Hooke hyperelastic material with isochoric and volumetric split.
*/

class NeoHookeElasticityIV : public NeoHookeElasticity
{
public:
  //! \brief The default constructor invokes the parent class constructor only.
  //! \param[in] n Number of spatial dimensions
  NeoHookeElasticityIV(unsigned short int n = 3)
  : NeoHookeElasticity(n), Siso(n) {}

  //! \brief Empty destructor.
  virtual ~NeoHookeElasticityIV() {}

  //! \brief Prints out problem definition to the given output stream.
  virtual void print(std::ostream& os) const;

protected:
  //! \brief Forms tangential tensorial quantities needed by the evalInt method.
  //! \param[in] X Cartesian coordinates of current integration point
  //! \param[out] Ctan Tangential constitutive tensor at current point
  //! \param[out] S 2nd Piola-Kirchhoff stress tensor at current point
  virtual bool formTangent(Matrix& Ctan, SymmTensor& S, const Vec3& X) const;

  //! \brief Forms the 2nd Piola-Kirchhoff stress tensor.
  virtual void formStressTensor(SymmTensor& S) const;

public:
  //! \brief Sets up the tangential constitutive matrix at current point.
  //! \param[out] Ctan \f$6\times6\f$-matrix (in 3D) or \f$3\times3\f$-matrix
  //! (in 2D), representing the tangential constitutive tensor
  //! \param[in] X Cartesian coordinates of current point
  virtual bool formCmatrix(Matrix& Ctan, const Vec3& X, bool = false) const;

protected:
  mutable SymmTensor Siso; //!< Isochoric part of the 2nd PK stress tensor
};

#endif
