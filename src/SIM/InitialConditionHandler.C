// $Id$
//==============================================================================
//!
//! \file InitialConditionHandler.C
//!
//! \date Oct 31 2012
//!
//! \author Arne Morten Kvarving / SINTEF
//!
//! \brief Functions for loading initial conditions
//!
//==============================================================================


#include "InitialConditionHandler.h"

#include "ASMbase.h"
#include "HDF5Writer.h"
#include "XMLWriter.h"

#include <sstream>


bool SIM::setInitialConditions (SIMbase& sim, SIMdependency* fieldHolder)
{
  bool result = false;

  if (!fieldHolder)
    fieldHolder = &sim;
  result = true;
  // loops over input files
  SIMdependency::InitialCondMap::const_iterator it;
  for (it = sim.getICs().begin(); it != sim.getICs().end(); ++it) {
    XMLWriter xmlreader(it->first);
    xmlreader.readInfo();
    HDF5Writer hdf5reader(it->first,true);
    std::map<std::string, SIMdependency::PatchVec> basis;
    // loops over ic's
    std::vector<SIMdependency::ICInfo>::const_iterator it2;
    for (it2 = it->second.begin(); it2 != it->second.end();++it2) {
      // do we have this field?
      std::stringstream str;
      str << it2->sim_field;
      str << -it2->sim_level+1; // 0 -> 1, -1 -> 2, ...
      utl::vector<double>* field = fieldHolder->getField(str.str());
      if (!field)
        continue;

      std::vector<XMLWriter::Entry>::const_iterator it3;
      // find entry in XML description file
      for (it3  = xmlreader.getEntries().begin();
           it3 != xmlreader.getEntries().end(); ++it3) {
        if (it3->name == it2->file_field)
          break;
      }
      if (it3 == xmlreader.getEntries().end()) {
        std::cerr << "Could not find IC (" << it2->file_field 
                                           << "," << it2->file_level
                                           << ") -> (" << it2->sim_field
                                           << ", " << it2->sim_level << ")"
                                           << std::endl;
        return false;
      }
      // load basis
      if (basis.find(it3->basis) == basis.end()) {
        SIMdependency::PatchVec vec;
        for (int i=0;i<it3->patches;++i) {
          std::stringstream str;
          str << it2->file_level << "/basis/" << it3->basis << "/" << i+1;
          std::string pg2;
          hdf5reader.readString(str.str(),pg2);
          std::stringstream spg2;
          spg2 << pg2;
          ASMbase* pch = sim.readPatch(spg2,i);
          if (pch)
            vec.push_back(pch);
        }
        basis.insert(make_pair(it3->basis, vec));
      }
      // loop over patches
      for (int i=0;i<it3->patches;++i) {
        int p = sim.getLocalPatchIndex(i+1);
        if (p < 1)
          continue;
        Vector loc, newloc;
        hdf5reader.readVector(it2->file_level, it2->file_field, i+1, loc);
        sim.getFEModel()[p-1]->evaluate(basis[it3->basis][p-1], loc, newloc);
        sim.getFEModel()[p-1]->injectNodeVec(newloc, *field, it3->components);
      }
    }

    // clean up patches
    std::map<std::string, SIMdependency::PatchVec>::iterator it3;
    for (it3 = basis.begin(); it3 != basis.end(); ++it3)
      for (size_t i=0;i<it3->second.size();++i)
        delete it3->second[i];
  }

  return result;
}