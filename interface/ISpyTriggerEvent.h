#ifndef ANALYZER_ISPY_TRIGGER_EVENT_H
#define ANALYZER_ISPY_TRIGGER_EVENT_H

# include "FWCore/Framework/interface/EDAnalyzer.h"
# include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

/*
  Dump TriggerEvent a la HLTEventAnalyzerAOD, TriggerSummaryAnalyzerAOD
*/

class ISpyTriggerEvent : public edm::EDAnalyzer
{
public:
  explicit ISpyTriggerEvent(const edm::ParameterSet&);
  virtual ~ISpyTriggerEvent(void) {}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag triggerEventTag_;
  edm::InputTag triggerResultsTag_;
  std::string processName_;
  std::string triggerName_;

  HLTConfigProvider hltConfig_;
  bool hltConfigProvided_;
};

#endif // ANALYZER_ISPY_TRIGGER_EVENT_H
