#ifndef ANALYZER_ISPY_CSCCORRELATEDCLCTDIGI_H
#define ANALYZER_ISPY_CSCCORRELATEDLCTDIGI_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/CSCDigi/interface/CSCCorrelatedLCTDigiCollection.h"

#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

class ISpyCSCCorrelatedLCTDigi : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpyCSCCorrelatedLCTDigi(const edm::ParameterSet&);
  virtual ~ISpyCSCCorrelatedLCTDigi(void){}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);

private:
  edm::InputTag		inputTag_;
  edm::EDGetTokenT<CSCCorrelatedLCTDigiCollection> digiToken_;
  
  edm::ESGetToken<CSCGeometry, MuonGeometryRecord> cscGeometryToken_;
  const CSCGeometry* cscGeometry_;

};

#endif // ANALYZER_ISPY_CSCCORRELATEDLCTDIGI_H
