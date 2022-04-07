#ifndef ANALYZER_ISPY_DTDIGI_H
#define ANALYZER_ISPY_DTDIGI_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/DTDigi/interface/DTDigiCollection.h"

#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

class ISpyDTDigi : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpyDTDigi(const edm::ParameterSet&);
  virtual ~ISpyDTDigi(void){}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
  edm::EDGetTokenT<DTDigiCollection> digiToken_;
  
  edm::ESGetToken<DTGeometry, MuonGeometryRecord> dtGeometryToken_;
  const DTGeometry* dtGeometry_;

};

#endif //ANALYZER_ISPY_DTDIGI_H
