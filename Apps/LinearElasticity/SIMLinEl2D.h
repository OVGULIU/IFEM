// $Id$
//==============================================================================
//!
//! \file SIMLinEl2D.h
//!
//! \date Feb 04 2010
//!
//! \author Knut Morten Okstad / SINTEF
//!
//! \brief Solution driver for 2D NURBS-based linear elastic FEM analysis.
//!
//==============================================================================

#ifndef _SIM_LIN_EL_2D_H
#define _SIM_LIN_EL_2D_H

#include "SIM2D.h"
#include "SIMenums.h"

class Elasticity;
class Material;


/*!
  \brief Driver class for 2D isogeometric FEM analysis of elasticity problems.
  \details The class incapsulates data and methods for solving linear elasticity
  problems using NURBS-based finite elements. It reimplements the parse method
  of the parent class.
*/

class SIMLinEl2D : public SIM2D
{
public:
  //! \brief Default constructor.
  SIMLinEl2D() : SIM2D(2) {}
  //! \brief The destructor frees the dynamically allocated material properties.
  virtual ~SIMLinEl2D();

  //! \brief Initializes the property containers of the model.
  virtual void clearProperties();

  //! \brief Performs some pre-processing tasks on the FE model.
  //! \details This method is reimplemented inserting a call to \a getIntegrand.
  //! This makes sure the integrand has been allocated in case of minimum input.
  virtual bool preprocess(const std::vector<int>& ignored, bool fixDup);

private:
  //! \brief Returns the actual integrand.
  Elasticity* getIntegrand();

protected:
  //! \brief Parses a data section from the input stream.
  //! \param[in] keyWord Keyword of current data section to read
  //! \param is The file stream to read from
  virtual bool parse(char* keyWord, std::istream& is);

  //! \brief Parses a data section from an XML element
  //! \param[in] elem The XML element to parse
  virtual bool parse(const TiXmlElement* elem);

  //! \brief Initializes material properties for integration of interior terms.
  //! \param[in] propInd Physical property index
  virtual bool initMaterial(size_t propInd);
  //! \brief Initializes the body load properties for current patch.
  //! \param[in] patchInd 1-based patch index
  virtual bool initBodyLoad(size_t patchInd);

  //! \brief Initializes for integration of Neumann terms for a given property.
  //! \param[in] propInd Physical property index
  virtual bool initNeumann(size_t propInd);

public:
  static bool planeStrain; //!< Plane strain/stress option
  static bool axiSymmetry; //!< Axisymmtry option

protected:
  std::vector<Material*> mVec; //!< Material data
};

#endif
