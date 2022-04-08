#ifndef ANALYZER_ISPY_CALO_GEOMETRY_H
#define ANALYZER_ISPY_CALO_GEOMETRY_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ESWatcher.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/HcalDetId/interface/HcalSubdetector.h"
#include <string>

class GeomDet;
class SbVec3f;
class CaloGeometry;
class IgDataStorage;
class IgCollectionItem;
class CaloGeometryRecord;

class ISpyCaloGeometry : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpyCaloGeometry(const edm::ParameterSet&);
  virtual ~ISpyCaloGeometry(void) {}
       
  virtual void analyze(const edm::Event&, const edm::EventSetup&);

private:

  void buildCalo3D(IgDataStorage*);
  void buildCaloRPhi(IgDataStorage*);
  void buildCaloRZ(IgDataStorage*);

  void build3D(IgDataStorage*, const std::string&, DetId::Detector, int);
  void buildEndcap3D(IgDataStorage*, const std::string&, DetId::Detector, int, int); 
  
  void buildRPhi(IgDataStorage*, const std::string&, DetId::Detector, int, double);
  void buildRZ(IgDataStorage*, const std::string&, DetId::Detector, int, double, double);

  const std::string subDetName(HcalSubdetector key);
  const std::string otherSubDetName(HcalOtherSubdetector key);

  bool caloGeomChanged_;

  edm::ESGetToken<CaloGeometry, CaloGeometryRecord> caloGeometryToken_;
  const CaloGeometry* caloGeom_;
  edm::ESWatcher<CaloGeometryRecord> watch_caloGeom_;

};

#endif // ANALYZER_ISPY_CALO_GEOMETRY_H
