#ifndef ANALYZER_ISPY_PFREC_TRACK_H
#define ANALYZER_ISPY_PFREC_TRACK_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

class ISpyPFRecTrack : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpyPFRecTrack(const edm::ParameterSet&);
  virtual ~ISpyPFRecTrack(void) {}
    
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
};

#endif // ANALYZER_ISPY_PFREC_TRACK_H
