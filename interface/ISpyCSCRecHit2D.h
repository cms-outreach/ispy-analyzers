#ifndef ANALYZER_ISPY_CSCRECHIT2D_H
#define ANALYZER_ISPY_CSCRECHIT2D_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/CSCRecHit/interface/CSCRecHit2DCollection.h"

#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

class ISpyCSCRecHit2D : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpyCSCRecHit2D(const edm::ParameterSet&);
  virtual ~ISpyCSCRecHit2D(void){}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
  edm::EDGetTokenT<CSCRecHit2DCollection> rechitToken_;
  
  edm::ESGetToken<CSCGeometry, MuonGeometryRecord> cscGeometryToken_;
  const CSCGeometry* cscGeometry_;

};

#endif // ANALYZER_ISPY_CSCRECHIT2D_H
