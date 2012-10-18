// $Id$
//==============================================================================
//!
//! \file SAMpatch.h
//!
//! \date Dec 10 2008
//!
//! \author Knut Morten Okstad / SINTEF
//!
//! \brief Assembly of FE matrices into system matrices for multi-patch models.
//!
//==============================================================================

#ifndef _SAM_PATCH_H
#define _SAM_PATCH_H

#include "SAM.h"
#include "ASMbase.h"

/*!
  \brief This is a sub-class of SAM with added functionality for spline models.
  \details It contains some additional functions for initializing the SAM-data
  for an FE model based on ASMbase patches.
*/

class SAMpatch : public SAM
{
public:
  //! \brief Empty default constructor.
  SAMpatch() {}
  //! \brief Empty destructor.
  virtual ~SAMpatch() {}

  //! \brief Allocates the dynamic arrays and populates them with data.
  //! \param[in] model All spline patches in the model
  //! \param[in] numNod Total number of unique nodes in the model
  virtual bool init(const std::vector<ASMbase*>& model, int numNod = 0);

  //! \brief Updates the multi-point constraint array \a TTCC.
  //! \param[in] model All spline patches in the model
  //! \param[in] prevSol Previous primary solution vector in DOF-order
  virtual bool updateConstraintEqs(const std::vector<ASMbase*>& model,
				   const Vector* prevSol = 0);

protected:
  //! \brief Initializes the nodal arrays \a MINEX, \a MADOF and \a MSC.
  bool initNodeDofs(const std::vector<ASMbase*>& model);
  //! \brief Initializes the element topology arrays \a MPMNPC and \a MMNPC.
  bool initElementConn(const std::vector<ASMbase*>& model);
  //! \brief Initializes the multi-point constraint arrays
  //! \a MPMCEQ, \a MMCEQ and \a TTCC.
  virtual bool initConstraintEqs(const std::vector<ASMbase*>& model);
};

#endif
