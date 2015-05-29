#include "L1TriggerDPGUpgrade/L1TMuon/interface/PtRefinementUnit.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

using namespace L1TMuon;

PtRefinementUnit::PtRefinementUnit(const edm::ParameterSet& ps):
  _name(ps.getParameter<std::string>("name")) {
}
