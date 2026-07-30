#ifndef ANALYZER_ISPY_PACKEDCANDIDATE_H
#define ANALYZER_ISPY_PACKEDCANDIDATE_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

class MagneticField;
class CaloGeometry;
class CaloGeometryRecord;

class ISpyPackedCandidate : public edm::one::EDAnalyzer<>
{
public:

  explicit ISpyPackedCandidate(const edm::ParameterSet&);
  virtual ~ISpyPackedCandidate(void){}
  virtual void analyze(const edm::Event&, const edm::EventSetup&);

private:

  edm::InputTag inputTag_;
  edm::EDGetTokenT<pat::PackedCandidateCollection> candidateToken_;
  
  edm::ESGetToken<MagneticField, IdealMagneticFieldRecord> magneticFieldToken_;    
  const MagneticField* magneticField_;
  
  edm::ESGetToken<CaloGeometry, CaloGeometryRecord> caloGeometryToken_;
  const CaloGeometry* caloGeom_;  

  int findArea(std::vector<std::array<RhoEtaPhi, 8> >&, double, double);
};
#endif // ANALYZER_ISPY_PACKEDCANDIDATE_H
