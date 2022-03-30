#ifndef ANALYZER_ISPY_CSCRECHIT2D_H
#define ANALYZER_ISPY_CSCRECHIT2D_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/CSCRecHit/interface/CSCRecHit2DCollection.h"

class ISpyCSCRecHit2D : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpyCSCRecHit2D(const edm::ParameterSet&);
  virtual ~ISpyCSCRecHit2D(void){}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
  edm::EDGetTokenT<CSCRecHit2DCollection> rechitToken_;
};

#endif // ANALYZER_ISPY_CSCRECHIT2D_H
