#ifndef ANALYZER_ISPY_EVENT_H
# define ANALYZER_ISPY_EVENT_H

# include "FWCore/Framework/interface/EDAnalyzer.h"

class ISpyEvent : public edm::EDAnalyzer
{
public:
  explicit ISpyEvent(const edm::ParameterSet&);
  virtual ~ISpyEvent(void) {}

  virtual void analyze(const edm::Event&, const edm::EventSetup&);
};

#endif // ANALYZER_ISPY_EVENT_H
