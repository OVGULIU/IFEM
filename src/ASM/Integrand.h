// $Id$
//==============================================================================
//!
//! \file Integrand.h
//!
//! \date Nov 11 2009
//!
//! \author Knut Morten Okstad / SINTEF
//!
//! \brief Abstract interface for classes representing FEM integrands.
//!
//==============================================================================

#ifndef _INTEGRAND_H
#define _INTEGRAND_H

#include <vector>
#include <cstddef>

struct TimeDomain;
class LocalIntegral;
class FiniteElement;
class MxFiniteElement;
class Vec3;


/*!
  \brief Abstract base class representing a system level integrated quantity.
  \details This class defines the interface between the finite element (FE)
  assembly drivers of the ASM-hierarchy and the problem-dependent classes
  containing all physical properties for the problem to be solved.

  The interface consists of methods for evaluating the integrand at interior
  integration points (\a evalInt), and at boundary integration points
  (\a evalBou). The latter are used for Neumann boundary conditions, typically.
  The integrand evaluation methods have access to the FE basis function values
  and derivatives through the FiniteElement argument. There are also a set
  of methods dedicated for mixed field interpolation problems, which take
  a MxFiniteElement object as argument instead.
*/

class Integrand
{
protected:
  //! \brief The default constructor is protected to allow sub-classes only.
  Integrand() {}

public:
  //! \brief Empty destructor.
  virtual ~Integrand() {}


  // Element-level initialization interface
  // ======================================

  //! \brief Returns a local integral contribution object for the given element.
  //! \param[in] nen Number of nodes on element
  //! \param[in] iEl Global element number (1-based)
  //! \param[in] neumann Whether or not we are assembling Neumann BCs
  virtual LocalIntegral* getLocalIntegral(size_t nen, size_t iEl,
                                          bool neumann = false) const = 0;
  //! \brief Returns a local integral contribution object for the given element.
  //! \param[in] nen1 Number of nodes on element for basis 1
  //! \param[in] nen2 Number of nodes on element for basis 2
  //! \param[in] iEl Global element number (1-based)
  //! \param[in] neumann Whether or not we are assembling Neumann BCs
  //!
  //! \details This form is used for mixed formulations only.
  //! The default implementation just forwards to the single-basis version.
  //! Reimplement this method if your mixed formulation requires specialized
  //! local integral objects.
  virtual LocalIntegral* getLocalIntegral(size_t nen1, size_t nen2,
                                          size_t iEl,
                                          bool neumann = false) const
  {
    return this->getLocalIntegral(nen1,iEl,neumann);
  }

  //! \brief Initializes current element for numerical integration.
  //! \param[in] MNPC Matrix of nodal point correspondance for current element
  //! \param[in] X0 Cartesian coordinates of the element center
  //! \param[in] nPt Number of integration points in this element
  //! \param elmInt Local integral for element
  //!
  //! \details This method is invoked once before starting the numerical
  //! integration loop over the Gaussian quadrature points over an element.
  //! It is supposed to perform all the necessary internal initializations
  //! needed before the numerical integration is started for current element.
  //! Reimplement this method for problems requiring the element center and/or
  //! the number of integration points during/before the integrand evaluations.
  virtual bool initElement(const std::vector<int>& MNPC,
			   const Vec3& X0, size_t nPt,
			   LocalIntegral& elmInt) = 0;
  //! \brief Initializes current element for numerical integration.
  //! \param[in] MNPC Matrix of nodal point correspondance for current element
  //! \param elmInt Local integral for element
  //!
  //! \details This method is invoked once before starting the numerical
  //! integration loop over the Gaussian quadrature points over an element.
  //! It is supposed to perform all the necessary internal initializations
  //! needed before the numerical integration is started for current element.
  //! Reimplement this method for problems \e not requiring the
  //! the element center nor the number of integration points before the
  //! integration loop is started.
  virtual bool initElement(const std::vector<int>& MNPC,
                           LocalIntegral& elmInt) = 0;
  //! \brief Initializes current element for numerical integration (mixed).
  //! \param[in] MNPC1 Nodal point correspondance for the basis 1
  //! \param[in] MNPC2 Nodal point correspondance for the basis 2
  //! \param[in] n1 Number of nodes in basis 1 on this patch
  //! \param elmInt Local integral for element
  virtual bool initElement(const std::vector<int>& MNPC1,
			   const std::vector<int>& MNPC2, size_t n1,
                           LocalIntegral& elmInt) = 0;

  //! \brief Initializes current element for boundary integration.
  //! \param[in] MNPC Matrix of nodal point correspondance for current element
  //! \param elmInt Local integral for element
  virtual bool initElementBou(const std::vector<int>& MNPC,
                              LocalIntegral& elmInt) = 0;
  //! \brief Initializes current element for boundary integration (mixed).
  //! \param[in] MNPC1 Nodal point correspondance for the basis 1
  //! \param[in] MNPC2 Nodal point correspondance for the basis 2
  //! \param[in] n1 Number of nodes in basis 1 on this patch
  //! \param elmInt Local integral for element
  virtual bool initElementBou(const std::vector<int>& MNPC1,
			      const std::vector<int>& MNPC2, size_t n1,
                              LocalIntegral& elmInt) = 0;


  // Integrand evaluation interface
  // ==============================

  //! \brief Enum defining the additional terms that an Integrand may require.
  enum Traits {
    STANDARD           = 0, //!< Default integrand type, no special terms
    SECOND_DERIVATIVES = 1, //!< Integrand wants second derivatives
    AVERAGE            = 2, //!< Integrand wants basis function averages
    ELEMENT_CORNERS    = 4, //!< Integrand wants element corner coordinates
    G_MATRIX           = 8, //!< Integrand wants the G matrix
    ELEMENT_CENTER     = 16,//!< Integrand wants element center coordinates
    XO_ELEMENTS        = 32 //!< Integrand is defined on extraordinary elements
  };

  //! \brief Defines which FE quantities are needed by the integrand.
  virtual int getIntegrandType() const { return STANDARD; }
  //! \brief Returns the number of reduced-order integration points.
  virtual int getReducedIntegration() const { return 0; }
  //! \brief Returns the number of boundary integration points.
  virtual int getBouIntegrationPoints(int nGP) const { return nGP; }

  //! \brief Evaluates reduced integration terms at an interior point.
  //! \param elmInt The local integral object to receive the contributions
  //! \param[in] fe Finite element data of current integration point
  //! \param[in] X Cartesian coordinates of current integration point
  //!
  //! \details Reimplement this method to evaluate terms at other points than
  //! the regular integration points. This method is invoked in a separate loop
  //! prior to the main integration point loop.
  virtual bool reducedInt(LocalIntegral& elmInt,
                          const FiniteElement& fe, const Vec3& X) const
  {
    return false;
  }

  //! \brief Evaluates the integrand at an interior point.
  //! \param elmInt The local integral object to receive the contributions
  //! \param[in] fe Finite element data of current integration point
  //! \param[in] time Parameters for nonlinear and time-dependent simulations
  //! \param[in] X Cartesian coordinates of current integration point
  //!
  //! \details The default implementation forwards to the stationary version.
  //! Reimplement this method for time-dependent or non-linear problems.
  virtual bool evalInt(LocalIntegral& elmInt, const FiniteElement& fe,
		       const TimeDomain& time, const Vec3& X) const
  {
    return this->evalInt(elmInt,fe,X);
  }

  //! \brief Evaluates the integrand at an interior point.
  //! \param elmInt The local integral object to receive the contributions
  //! \param[in] fe Mixed finite element data of current integration point
  //! \param[in] time Parameters for nonlinear and time-dependent simulations
  //! \param[in] X Cartesian coordinates of current integration point
  //!
  //! \details This interface is used for mixed formulations only.
  //! The default implementation forwards to the stationary version.
  //! Reimplement this method for time-dependent or non-linear problems.
  virtual bool evalIntMx(LocalIntegral& elmInt, const MxFiniteElement& fe,
			 const TimeDomain& time, const Vec3& X) const
  {
    return this->evalIntMx(elmInt,fe,X);
  }

  //! \brief Finalizes the element quantities after the numerical integration.
  //! \details This method is invoked once for each element, after the numerical
  //! integration loop over interior points is finished and before the resulting
  //! element quantities are assembled into their system level equivalents.
  //! It can also be used to implement multiple integration point loops within
  //! the same element, provided the necessary integration point values are
  //! stored internally in the object during the first integration loop.
  virtual bool finalizeElement(LocalIntegral&, const TimeDomain&, size_t = 0)
  {
    return true;
  }

  //! \brief Evaluates the integrand at a boundary point.
  //! \param elmInt The local integral object to receive the contributions
  //! \param[in] fe Finite element data of current integration point
  //! \param[in] time Parameters for nonlinear and time-dependent simulations
  //! \param[in] X Cartesian coordinates of current integration point
  //! \param[in] normal Boundary normal vector at current integration point
  //!
  //! \details The default implementation forwards to the stationary version.
  //! Reimplement this method for time-dependent or non-linear problems.
  virtual bool evalBou(LocalIntegral& elmInt, const FiniteElement& fe,
		       const TimeDomain& time,
		       const Vec3& X, const Vec3& normal) const
  {
    return this->evalBou(elmInt,fe,X,normal);
  }

  //! \brief Evaluates the integrand at a boundary point.
  //! \param elmInt The local integral object to receive the contributions
  //! \param[in] fe Mixed finite element data of current integration point
  //! \param[in] time Parameters for nonlinear and time-dependent simulations
  //! \param[in] X Cartesian coordinates of current integration point
  //! \param[in] normal Boundary normal vector at current integration point
  //!
  //! \details This interface is used for mixed formulations.
  //! The default implementation forwards to the stationary version.
  //! Reimplement this method for time-dependent or non-linear problems.
  virtual bool evalBouMx(LocalIntegral& elmInt, const MxFiniteElement& fe,
			 const TimeDomain& time,
			 const Vec3& X, const Vec3& normal) const
  {
    return this->evalBouMx(elmInt,fe,X,normal);
  }

protected:
  //! \brief Evaluates the integrand at interior points for stationary problems.
  virtual bool evalInt(LocalIntegral&, const FiniteElement& fe,
		       const Vec3&) const { return false; }
  //! \brief Evaluates the integrand at interior points for stationary problems.
  virtual bool evalIntMx(LocalIntegral&, const MxFiniteElement& fe,
			 const Vec3&) const { return false; }

  //! \brief Evaluates the integrand at boundary points for stationary problems.
  virtual bool evalBou(LocalIntegral&, const FiniteElement&,
		       const Vec3&, const Vec3&) const { return false; }
  //! \brief Evaluates the integrand at boundary points for stationary problems.
  virtual bool evalBouMx(LocalIntegral&, const MxFiniteElement&,
			 const Vec3&, const Vec3&) const { return false; }
};

#endif
