#ifndef ANALYZER_ISPY_CSCSTRIPDIGI_H
#define ANALYZER_ISPY_CSCSTRIPDIGI_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/CSCDigi/interface/CSCStripDigiCollection.h"

#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

class ISpyCSCStripDigi : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpyCSCStripDigi(const edm::ParameterSet&);
  virtual ~ISpyCSCStripDigi(void) {}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag 		inputTag_;
  int 				thresholdOffset_;
  edm::EDGetTokenT<CSCStripDigiCollection> digiToken_;
  
  edm::ESGetToken<CSCGeometry, MuonGeometryRecord> cscGeometryToken_;
  const CSCGeometry* cscGeometry_;

};

#endif //ANALYZER_ISPY_CSCSTRIPDIGI_H
