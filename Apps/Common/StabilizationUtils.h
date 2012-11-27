#ifndef STABILIZATIONUTILS_H_
#define STABILIZATIONUTILS_H_

#include <vector>
#include "Vec3.h"
#include "MatVec.h"

namespace StabilizationUtils {
  //! \brief Returns characteristic element size
  //! \param XC The element corner coordinates
  //! \details The size is taken as the shortest edge length
  double getElementSize(const std::vector<Vec3>& XC, int nsd);

  //! \brief Returns stabilization parameters for convecction-diffusion equation
  //! \param[in] dt The timestep size
  //! \param[in] mu Diffusion/viscosity parameter
  //! \param[in] U  Velocity vector
  //! \param[in] G  G matrix
  //! \details Stabilization parameter in integration point
  double getTauPt(double dt, double mu, const Vector& U, const Matrix& G);
 		  
  //! \brief Computes stabilization parameters for Navier-Stokes equations
  //! \param[in] dt The timestep size
  //! \param[in] mu Diffusion/viscosity parameter
  //! \param[in] U  Velocity vector
  //! \param[in] G  The G matrix
  //! \param[out] tauM Stabilization parameter for momentum
  //! \param[out] tauC Stabilization parameter for continuity
  //! \details Stabilization parameters in integration point
  bool getTauNSPt(double dt, double mu, const Vector& U, const Matrix& G,
		  double& tauM, double& tauC); 
}

#endif