#ifndef ANALYZER_ISPY_GEMRECHIT_H
#define ANALYZER_ISPY_GEMRECHIT_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/GEMRecHit/interface/GEMRecHitCollection.h"

#include "Geometry/Records/interface/MuonGeometryRecord.h"

class GEMGeometry;

class ISpyGEMRecHit : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpyGEMRecHit(const edm::ParameterSet&);
  virtual ~ISpyGEMRecHit(void){}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
  edm::EDGetTokenT<GEMRecHitCollection> rechitToken_;
  
  edm::ESGetToken<GEMGeometry, MuonGeometryRecord> gemGeometryToken_;
  const GEMGeometry* gemGeometry_;
  
};

#endif // ANALYZER_ISPY_GEMRECHIT_H
