#include "ISpy/Analyzers/interface/ISpyPhoton.h"
#include "ISpy/Analyzers/interface/ISpyService.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/EgammaCandidates/interface/Photon.h"
 
using namespace edm::service;
using namespace edm;
using namespace reco;

ISpyPhoton::ISpyPhoton(const edm::ParameterSet& iConfig)
: inputTag_(iConfig.getParameter<edm::InputTag>("iSpyPhotonTag"))
{
  photonToken_ = consumes<PhotonCollection>(inputTag_);
}

void ISpyPhoton::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;
  
  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyPhoton requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  edm::Handle<PhotonCollection> collection;
  event.getByToken(photonToken_, collection);

  if ( collection.isValid() )
  {
    std::string product = "Photons "
                          + edm::TypeID (typeid (PhotonCollection)).friendlyClassName() + ":"
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + ":"
                          + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection& photons = storage->getCollection("Photons_V1");
      
    IgProperty E   = photons.addProperty("energy", 0.0);
    IgProperty ET  = photons.addProperty("et", 0.0);   
    IgProperty ETA = photons.addProperty("eta", 0.0);
    IgProperty PHI = photons.addProperty("phi", 0.0);
    IgProperty POS = photons.addProperty("pos", IgV3d());

    // Shower shape variables
    IgProperty HOE  = photons.addProperty("hadronicOverEm", 0.0);
    IgProperty HD1  = photons.addProperty("hadronicDepth1OverEcal", 0.0);
    IgProperty HD2  = photons.addProperty("hadronicDepth2OverEcal", 0.0);
    IgProperty E1X5 = photons.addProperty("e1x5", 0.0);
    IgProperty E2X5 = photons.addProperty("e2x5", 0.0);
    IgProperty E3X3 = photons.addProperty("e3x3", 0.0);
    IgProperty E5X5 = photons.addProperty("e5x5", 0.0);
    IgProperty MEX  = photons.addProperty("maxEnergyXtal", 0.0);
    IgProperty SEE  = photons.addProperty("sigmaEtaEta", 0.0);
    IgProperty SII  = photons.addProperty("sigmaIetaIeta", 0.0);
    IgProperty R1X5 = photons.addProperty("r1x5", 0.0);
    IgProperty R2X5 = photons.addProperty("r2x5", 0.0);
    IgProperty R9   = photons.addProperty("r9", 0.0);

    // Isolation variables
    // NOTE: There are 2 sets, one for dR = 0.4 and another for dR = 0.3
    IgProperty ESDR4  = photons.addProperty("ecalRecHitSumEtConeDR04", 0.0);
    IgProperty HSDR4  = photons.addProperty("hcalTowerSumEtConeDR04", 0.0);
    IgProperty HD1DR4 = photons.addProperty("hcalDepth1TowerSumEtConeDR04", 0.0);
    IgProperty HD2DR4 = photons.addProperty("hcalDepth2TowerSumEtConeDR04", 0.0);
    IgProperty TSSDR4 = photons.addProperty("trkSumPtSolidConeConeDR04", 0.0);
    IgProperty TSHDR4 = photons.addProperty("trkSumPtHollowConeDR04", 0.0);
    IgProperty NTSDR4 = photons.addProperty("nTrkSolidConeDR04", int(0));
    IgProperty NTHDR4 = photons.addProperty("nTrkHollowConeDR04", int(0));

    IgProperty ESDR3  = photons.addProperty("ecalRecHitSumEtDR03", 0.0);
    IgProperty HSDR3  = photons.addProperty("hcalTowerSumEtDR03", 0.0);
    IgProperty HD1DR3 = photons.addProperty("hcalDepth1TowerSumEtDR03", 0.0);
    IgProperty HD2DR3 = photons.addProperty("hcalDepth2TowerSumEtDR03", 0.0);
    IgProperty TSSDR3 = photons.addProperty("trkSumPtSolidConeDR03", 0.0);
    IgProperty TSHDR3 = photons.addProperty("trkSumPtHollowConeDR03", 0.0);
    IgProperty NTSDR3 = photons.addProperty("nTrkSolidConeDR03", int(0));
    IgProperty NTHDR3 = photons.addProperty("nTrkHollowConeDR03", int(0));

    for ( PhotonCollection::const_iterator pi = collection->begin(), pie = collection->end(); 
          pi != pie; ++pi )
    {
      IgCollectionItem p = photons.create();

      p[E] = pi->energy();
      p[ET] = pi->et();
      p[ETA] = pi->eta();
      p[PHI] = pi->phi();
      p[POS] = IgV3d(pi->vx()/100.0, pi->vy()/100.0, pi->vz()/100.0);

      // NOTE: Need to cast float to double
      p[HOE]  = static_cast<double>(pi->hadronicOverEm());
      p[HD1]  = static_cast<double>(pi->hadronicDepth1OverEm());
      p[HD2]  = static_cast<double>(pi->hadronicDepth2OverEm());
      p[E1X5] = static_cast<double>(pi->e1x5()); 
      p[E2X5] = static_cast<double>(pi->e2x5());
      p[E3X3] = static_cast<double>(pi->e3x3()); 
      p[E5X5] = static_cast<double>(pi->e5x5());
      p[MEX]  = static_cast<double>(pi->maxEnergyXtal()); 
      p[SEE]  = static_cast<double>(pi->sigmaEtaEta());
      p[SII]  = static_cast<double>(pi->sigmaIetaIeta()); 
      p[R1X5] = static_cast<double>(pi->r1x5());
      p[R2X5] = static_cast<double>(pi->r2x5()); 
      p[R9]   = static_cast<double>(pi->r9());

      p[ESDR4]  = static_cast<double>(pi->ecalRecHitSumEtConeDR04());     
      p[HSDR4]  = static_cast<double>(pi->hcalTowerSumEtConeDR04());    
      p[HD1DR4] = static_cast<double>(pi->hcalDepth1TowerSumEtConeDR04());     
      p[HD2DR4] = static_cast<double>(pi->hcalDepth2TowerSumEtConeDR04());    
      p[TSSDR4] = static_cast<double>(pi->trkSumPtSolidConeDR04());   
      p[TSHDR4] = static_cast<double>(pi->trkSumPtHollowConeDR04());   
      p[NTSDR4] = pi->nTrkSolidConeDR04();             
      p[NTHDR4] = pi->nTrkHollowConeDR04();           

      p[ESDR3]  = static_cast<double>(pi->ecalRecHitSumEtConeDR03());     
      p[HSDR3]  = static_cast<double>(pi->hcalTowerSumEtConeDR03());    
      p[HD1DR3] = static_cast<double>(pi->hcalDepth1TowerSumEtConeDR03());     
      p[HD2DR3] = static_cast<double>(pi->hcalDepth2TowerSumEtConeDR03());    
      p[TSSDR3] = static_cast<double>(pi->trkSumPtSolidConeDR03());   
      p[TSHDR3] = static_cast<double>(pi->trkSumPtHollowConeDR03());   
      p[NTSDR3] = pi->nTrkSolidConeDR03();             
      p[NTHDR3] = pi->nTrkHollowConeDR03();   
    } 
  }
  else
  {
    std::string error = "### Error: Photons "
                        + edm::TypeID (typeid (PhotonCollection)).friendlyClassName() + ":"
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":"
                        + inputTag_.process() + " are not found.";
    config->error (error);
  }
}

DEFINE_FWK_MODULE(ISpyPhoton);
