// $Id$
//==============================================================================
//!
//! \file SIMCoupled.h
//!
//! \date Oct 12 2012
//!
//! \author Arne Morten Kvarving / SINTEF
//!
//! \brief Coupled SIM solver class template.
//!
//==============================================================================

#ifndef _SIM_COUPLED_H_
#define _SIM_COUPLED_H_

#include "SIMdependency.h"

class DataExporter;
class TimeStep;


/*!
  \brief Template class for coupled simulators.
*/

template<class T1, class T2> class SIMCoupled
{
public:
  //! \brief The constructor initializes the references to the two solvers.
  SIMCoupled(T1& s1, T2& s2) : S1(s1), S2(s2) {}

  //! \brief Empty destructor.
  virtual ~SIMCoupled() {}

  //! \brief Sets up field dependencies.
  virtual void setupDependencies() = 0;

  //! \brief Performs some pre-processing tasks on the FE model.
  bool preprocess()
  {
    return S1.preprocess() && S2.preprocess();
  }

  //! \brief Advances the time step one step forward.
  bool advanceStep(TimeStep& tp)
  {
    return S1.advanceStep(tp) && S2.advanceStep(tp);
  }

  //! \brief Computes the solution for the current time step.
  bool solveStep(TimeStep& tp)
  {
    return S1.solveStep(tp) && S2.solveStep(tp);
  }

  //! \brief Saves the converged results to VTF-file of a given time step.
  bool saveStep(const TimeStep& tp, int& nBlock)
  {
    return S2.saveStep(tp,nBlock) && S1.saveStep(tp,nBlock);
  }

  //! \brief Opens a new VTF-file and writes the model geometry to it.
  bool saveModel(char* fileName, int& nBlock)
  {
    if (!S1.saveModel(fileName,nBlock))
      return false;

    S2.setVTF(S1.getVTF());
    return true;
  }

  //! \brief Initializes for time-dependent simulation.
  bool init(const TimeStep& tp)
  {
    return S1.init(tp) && S2.init(tp);
  }

  //! \brief Registers a dependency on a field from another SIM object.
  virtual void registerDependency(SIMdependency* sim, const std::string& name,
                                  short int nvc,
                                  const SIMdependency::PatchVec& patches,
                                  bool diffBasis = false)
  {
    S1.registerDependency(sim, name, nvc, patches, diffBasis);
    S2.registerDependency(sim, name, nvc, patches, diffBasis);
  }

  //! \brief Registers a dependency on a field from another SIM object.
  void registerDependency(SIMdependency* sim, const std::string& name,
                          short int nvc = 1)
  {
    S1.registerDependency(sim, name, nvc);
    S2.registerDependency(sim, name, nvc);
  }

  //! \brief Returns a unique integer code for a Property set.
  int getUniquePropertyCode(const std::string& setName, int comp = 0)
  {
    return S1.getUniquePropertyCode(setName, comp);
  }

  //! \brief Creates a set of Property objects.
  bool createPropertySet(const std::string& setName, int pc)
  {
    return S1.createPropertySet(setName, pc);
  }

  //! \brief Defines a vector field property.
  size_t setVecProperty(int code, Property::Type ptype, VecFunc* field = NULL,
			int pflag = -1)
  {
    return S1.setVecProperty(code, ptype, field, pflag);
  }

  //! \brief Registers the field vectors for storage on HDF5 output.
  void registerFields(DataExporter& exporter)
  {
    S1.registerFields(exporter);
    S2.registerFields(exporter);
  }

  void setInitialConditions()
  {
    S1.setInitialConditions();
    S2.setInitialConditions();
  }

  bool hasIC(const std::string& name) const
  {
    return S1.hasIC(name) || S2.hasIC(name);
  }

  utl::vector<double>* getField(const std::string& name)
  {
    utl::vector<double>* result = S1.getField(name);
    if (!result)
      result = S2.getField(name);

    return result;
  }
protected:
  T1& S1; //!< First substep
  T2& S2; //!< Second substep
};

#endif
