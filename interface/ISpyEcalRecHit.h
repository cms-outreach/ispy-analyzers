#ifndef ANALYZER_ISPY_ECAL_REC_HIT_H
#define ANALYZER_ISPY_ECAL_REC_HIT_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

typedef std::vector<edm::InputTag> VInputTag;

class ISpyEcalRecHit : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpyEcalRecHit(const edm::ParameterSet&);
  virtual ~ISpyEcalRecHit(void) {}

  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  VInputTag	inputTags_;
};

#endif // ANALYZER_ISPY_ECAL_REC_HIT_H
