#ifndef ANALYZER_ISPY_PACKEDCANDIDATE_H
#define ANALYZER_ISPY_PACKEDCANDIDATE_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

class ISpyPackedCandidate : public edm::EDAnalyzer
{
public:
  explicit ISpyPackedCandidate(const edm::ParameterSet&);
  virtual ~ISpyPackedCandidate(void){}
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
  edm::EDGetTokenT<pat::PackedCandidateCollection> candidateToken_;
};
#endif // ANALYZER_ISPY_PACKEDCANDIDATE_H
