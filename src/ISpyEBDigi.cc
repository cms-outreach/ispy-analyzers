#include "ISpy/Analyzers/interface/ISpyEBDigi.h"
#include "ISpy/Analyzers/interface/ISpyService.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"

#include "ISpy/Services/interface/IgCollection.h"

#include "Geometry/EcalMapping/interface/EcalElectronicsMapping.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

using namespace edm::service;
using namespace edm;

ISpyEBDigi::ISpyEBDigi(const edm::ParameterSet& iConfig)
  : inputDigiTag_(iConfig.getParameter<edm::InputTag>("iSpyEBDigiTag")),
    inputRecHitTag_(iConfig.getParameter<edm::InputTag>("iSpyEBRecHitTag"))
{
  digiToken_ = consumes<EBDigiCollection>(inputDigiTag_);
  rechitToken_ = consumes<EcalRecHitCollection>(inputRecHitTag_);
}

void ISpyEBDigi::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyEBDigi requires the ISpyService\n"
     "which is not present in the configuration file.\n"
     "You must add the service in the configuration file\n"
     "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  edm::ESHandle<CaloGeometry> geom;
  eventSetup.get<CaloGeometryRecord> ().get(geom);

  if ( ! geom.isValid() )
  {
    std::string error = 
      "### Error: ISpyEBDigi::analyze: Invalid CaloGeometryRecord ";
    config->error (error);
    return;
  }

  edm::Handle<EBDigiCollection> digiCollection;
  event.getByToken(digiToken_, digiCollection);

  if ( ! digiCollection.isValid() )
  {
    std::string error = "### Error: EBDigis "
                        + edm::TypeID (typeid (EBDigiCollection)).friendlyClassName() + ":"
                        + inputDigiTag_.label() + ":"
                        + inputDigiTag_.instance() + " are not found";
    config->error(error);
    return;
  }
  
  edm::Handle<EcalRecHitCollection> recHitCollection;
  event.getByToken(rechitToken_, recHitCollection);

  if ( ! recHitCollection.isValid() )
  {
    std::string error = "### Error: EBRecHits "
			+ edm::TypeID (typeid (EcalRecHitCollection)).friendlyClassName() + ":" 
			+ inputRecHitTag_.label() + ":"
			+ inputRecHitTag_.instance() + ":" 
			+ inputRecHitTag_.process() + " are not found.";
    config->error(error);
    return;
  }
  
  else
  {
    std::string product = "EBDigis "
                          + edm::TypeID (typeid (EBDigiCollection)).friendlyClassName() + ":"
                          + inputDigiTag_.label() + ":" 
                          + inputDigiTag_.instance() + ":"
                          + inputDigiTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection& digis = storage->getCollection("EBDigis_V1");
     
    IgProperty E     = digis.addProperty("energy", 0.0);
    IgProperty ETA   = digis.addProperty("eta",    0.0);
    IgProperty PHI   = digis.addProperty("phi",    0.0);
    IgProperty TIME  = digis.addProperty("time",   0.0);
    IgProperty DETID = digis.addProperty("detid",  int(0));

    IgProperty ADC0 = digis.addProperty("adc0", int(0));
    IgProperty ADC1 = digis.addProperty("adc1", int(0));
    IgProperty ADC2 = digis.addProperty("adc2", int(0));
    IgProperty ADC3 = digis.addProperty("adc3", int(0));
    IgProperty ADC4 = digis.addProperty("adc4", int(0));
    IgProperty ADC5 = digis.addProperty("adc5", int(0));
    IgProperty ADC6 = digis.addProperty("adc6", int(0));
    IgProperty ADC7 = digis.addProperty("adc7", int(0));
    IgProperty ADC8 = digis.addProperty("adc8", int(0));
    IgProperty ADC9 = digis.addProperty("adc9", int(0));

    IgProperty FRONT_1 = digis.addProperty("front_1", IgV3d());
    IgProperty FRONT_2 = digis.addProperty("front_2", IgV3d());      
    IgProperty FRONT_3 = digis.addProperty("front_3", IgV3d());      
    IgProperty FRONT_4 = digis.addProperty("front_4", IgV3d());
    IgProperty BACK_1  = digis.addProperty("back_1",  IgV3d());
    IgProperty BACK_2  = digis.addProperty("back_2",  IgV3d());
    IgProperty BACK_3  = digis.addProperty("back_3",  IgV3d());
    IgProperty BACK_4  = digis.addProperty("back_4",  IgV3d());
   
    for ( EBDigiCollection::const_iterator di = digiCollection->begin(), diEnd = digiCollection->end(); 
          di != diEnd; ++di ) 
    {
      IgCollectionItem d = digis.create();  

      auto cell = (*geom).getGeometry((*di).id());
      const CaloCellGeometry::CornersVec& corners = cell->getCorners();
      const GlobalPoint& pos = cell->getPosition();
 
      EBDetId detid = EBDetId((*di).id());
      const EcalRecHit recHit = *(recHitCollection->find(detid));
   
      d[E] = static_cast<double>(recHit.energy());
      d[ETA] = static_cast<double>(pos.eta());
      d[PHI] = static_cast<double>(pos.phi());
      d[TIME] = static_cast<double>(recHit.time());
      d[DETID] = static_cast<int>((*di).id());

      assert((*di).size() == 10);

      EBDataFrame df(*di);
      d[ADC0] = df.sample(0).adc();
      d[ADC1] = df.sample(1).adc();
      d[ADC2] = df.sample(2).adc();
      d[ADC3] = df.sample(3).adc();
      d[ADC4] = df.sample(4).adc();
      d[ADC5] = df.sample(5).adc();
      d[ADC6] = df.sample(6).adc();
      d[ADC7] = df.sample(7).adc();
      d[ADC8] = df.sample(8).adc();
      d[ADC9] = df.sample(9).adc();

      assert(corners.size() == 8);

      d[FRONT_1] = IgV3d(static_cast<double>(corners[3].x()/100.0), 
                         static_cast<double>(corners[3].y()/100.0), 
                         static_cast<double>(corners[3].z()/100.0));
      d[FRONT_2] = IgV3d(static_cast<double>(corners[2].x()/100.0), 
                         static_cast<double>(corners[2].y()/100.0), 
                         static_cast<double>(corners[2].z()/100.0));
      d[FRONT_3] = IgV3d(static_cast<double>(corners[1].x()/100.0), 
                         static_cast<double>(corners[1].y()/100.0), 
                         static_cast<double>(corners[1].z()/100.0));
      d[FRONT_4] = IgV3d(static_cast<double>(corners[0].x()/100.0), 
                         static_cast<double>(corners[0].y()/100.0), 
                         static_cast<double>(corners[0].z()/100.0));
           
      d[BACK_1] = IgV3d(static_cast<double>(corners[7].x()/100.0), 
                        static_cast<double>(corners[7].y()/100.0), 
                        static_cast<double>(corners[7].z()/100.0));
      d[BACK_2] = IgV3d(static_cast<double>(corners[6].x()/100.0), 
                        static_cast<double>(corners[6].y()/100.0), 
                        static_cast<double>(corners[6].z()/100.0));
      d[BACK_3] = IgV3d(static_cast<double>(corners[5].x()/100.0), 
                        static_cast<double>(corners[5].y()/100.0), 
                        static_cast<double>(corners[5].z()/100.0));
      d[BACK_4] = IgV3d(static_cast<double>(corners[4].x()/100.0), 
                        static_cast<double>(corners[4].y()/100.0), 
                        static_cast<double>(corners[4].z()/100.0));

    }
  }
}

DEFINE_FWK_MODULE(ISpyEBDigi);
