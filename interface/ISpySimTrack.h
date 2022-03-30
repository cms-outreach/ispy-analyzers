#ifndef ANALYZER_ISPY_SIM_TRACK_H
#define ANALYZER_ISPY_SIM_TRACK_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include <map>

class PSimHit;

typedef std::vector<edm::InputTag> VInputTag;
typedef std::map<int, std::vector<PSimHit> > SimHits;

class ISpySimTrack : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpySimTrack(const edm::ParameterSet&); 
  virtual ~ISpySimTrack(void) {}
       
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  VInputTag trackTags_;
  VInputTag vertexTags_;
  VInputTag hitTags_;

  // first: track ID, second: PSimHit
  SimHits simHits_;
};

#endif // ANALYZER_ISPY_SIM_TRACK_H
