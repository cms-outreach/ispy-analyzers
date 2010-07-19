#ifndef ANALYZER_ISPY_TRACK_H
#define ANALYZER_ISPY_TRACK_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include <vector>

typedef std::vector<edm::InputTag> VInputTag;

class ISpyTrack : public edm::EDAnalyzer
{
public:
  explicit ISpyTrack(const edm::ParameterSet&);
  virtual ~ISpyTrack(void) {}
       
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  VInputTag	inputTags_;
};

#endif // ANALYZER_ISPY_TRACK_H
