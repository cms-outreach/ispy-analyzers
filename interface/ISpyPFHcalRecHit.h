#ifndef ANALYZER_ISPY_PFHCALRECHIT_H
#define ANALYZER_ISPY_PFHCALRECHIT_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/ParticleFlowReco/interface/PFRecHitFwd.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

class ISpyPFHcalRecHit : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpyPFHcalRecHit(const edm::ParameterSet&);
  virtual ~ISpyPFHcalRecHit(void) {}
    
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:

  edm::InputTag hbheInputTag_;
  edm::InputTag hfInputTag_;
  edm::InputTag hoInputTag_;

  edm::EDGetTokenT<reco::PFRecHitCollection> hbheToken_;        
  edm::EDGetTokenT<reco::PFRecHitCollection> hfToken_;
  edm::EDGetTokenT<reco::PFRecHitCollection> hoToken_;

  edm::ESGetToken<CaloGeometry, CaloGeometryRecord> caloGeometryToken_;
  const CaloGeometry* caloGeometry_;
};

#endif // ANALYZER_ISPY_PFHCALRECHIT_H
