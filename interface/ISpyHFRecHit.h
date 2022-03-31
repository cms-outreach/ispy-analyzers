#ifndef ANALYZER_ISPY_HFREC_HIT_H
#define ANALYZER_ISPY_HFREC_HIT_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

class ISpyHFRecHit : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpyHFRecHit(const edm::ParameterSet&);
  virtual ~ISpyHFRecHit(void) {}
       
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag	inputTag_;
  edm::EDGetTokenT<HFRecHitCollection> rechitToken_;
  edm::ESGetToken<CaloGeometry, CaloGeometryRecord> caloGeometryToken_;
  const CaloGeometry* caloGeometry_;

};

#endif // ANALYZER_ISPY_HFREC_HIT_H
