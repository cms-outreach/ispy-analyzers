#ifndef ANALYZER_ISPY_RPCREC_HIT_H
#define ANALYZER_ISPY_RPCREC_HIT_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/RPCRecHit/interface/RPCRecHitCollection.h"

#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"

class ISpyRPCRecHit : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpyRPCRecHit(const edm::ParameterSet&);
  virtual ~ISpyRPCRecHit(void) {}
       
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag	inputTag_;
  edm::EDGetTokenT<RPCRecHitCollection> rechitToken_;
  edm::ESGetToken<RPCGeometry, MuonGeometryRecord>  rpcGeometryToken_;
  const RPCGeometry* rpcGeometry_;

};

#endif // ANALYZER_ISPY_RPCREC_HIT_H
