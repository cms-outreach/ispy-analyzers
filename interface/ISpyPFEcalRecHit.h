#ifndef ANALYZER_ISPY_PFECALRECHIT_H
#define ANALYZER_ISPY_PFECALRECHIT_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/ParticleFlowReco/interface/PFRecHitFwd.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

class ISpyPFEcalRecHit : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpyPFEcalRecHit(const edm::ParameterSet&);
  virtual ~ISpyPFEcalRecHit(void) {}
    
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:

  edm::InputTag inputTag_;
  edm::EDGetTokenT<reco::PFRecHitCollection> rechitToken_;
  edm::ESGetToken<CaloGeometry, CaloGeometryRecord> caloGeometryToken_;
  const CaloGeometry* caloGeometry_;

};

#endif // ANALYZER_ISPY_PFECALRECHIT_H
