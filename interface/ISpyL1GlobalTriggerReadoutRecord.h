#ifndef ANALYZER_ISPY_L1_GLOBAL_TRIGGER_READOUT_RECORD_H
#define ANALYZER_ISPY_L1_GLOBAL_TRIGGER_READOUT_RECORD_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

class ISpyL1GlobalTriggerReadoutRecord : public edm::EDAnalyzer
{
public:
  explicit ISpyL1GlobalTriggerReadoutRecord(const edm::ParameterSet&);
  virtual ~ISpyL1GlobalTriggerReadoutRecord(void) {}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
};
#endif // ANALYZER_ISPY_L1_GLOBAL_TRIGGER_READOUT_RECORD_H
