#include "ISpy/Analyzers/interface/ISpyTriggerEvent.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/Common/interface/TriggerResults.h"

#include <sstream>

using namespace edm::service;
using namespace edm;
using namespace reco;
using namespace trigger;

ISpyTriggerEvent::ISpyTriggerEvent(const edm::ParameterSet& iConfig)
  : triggerEventTag_(iConfig.getParameter<edm::InputTag>("triggerEventTag")),
    triggerResultsTag_(iConfig.getParameter<edm::InputTag>("triggerResultsTag")),
    processName_(iConfig.getParameter<std::string>("processName")),
    hltConfigProvided_(true)
{
  if ( ! hltConfig_.init(processName_) )
    hltConfigProvided_ = false;
}

void ISpyTriggerEvent::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyTriggerEvent requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  // FIXME: we should perhaps have a generic error class

  if ( ! hltConfigProvided_ )
  {
    std::string error = 
      "### Error: ISpyTriggerEvent: cannot initialize HLTConfigProvider with process name "+processName_;

    IgDataStorage* storage = config->storage();
    IgCollection& collection = storage->getCollection("Errors_V1");
    IgProperty ERROR_MSG = collection.addProperty("Error", std::string());
    IgCollectionItem item = collection.create();
    item[ERROR_MSG] = error;
    
    return;
  }
    
  Handle<TriggerEvent> triggerEvent;
  event.getByLabel(triggerEventTag_, triggerEvent);

  Handle<TriggerResults> triggerResults;
  event.getByLabel(triggerResultsTag_, triggerResults);

  if ( ! triggerEvent.isValid() )
  {
    std::string error = "### Error: TriggerEvent "
                        + edm::TypeID (typeid (TriggerEvent)).friendlyClassName() + ":"
                        + triggerEventTag_.label() + ":"
                        + triggerEventTag_.instance() + ":"
                        + triggerEventTag_.process() + " not found.";

    IgDataStorage* storage = config->storage();
    IgCollection& collection = storage->getCollection("Errors_V1");
    IgProperty ERROR_MSG = collection.addProperty("Error", std::string());
    IgCollectionItem item = collection.create();
    item[ERROR_MSG] = error;
  }
    
  if ( ! triggerResults.isValid() )
  {
    std::string error = "### Error: TriggerResults "
                        + edm::TypeID (typeid (TriggerResults)).friendlyClassName() + ":"
                        + triggerResultsTag_.label() + ":"
                        + triggerResultsTag_.instance() + ":"
                        + triggerResultsTag_.process() + " not found.";

    IgDataStorage* storage = config->storage();
    IgCollection& collection = storage->getCollection("Errors_V1");
    IgProperty ERROR_MSG = collection.addProperty("Error", std::string());
    IgCollectionItem item = collection.create();
    item[ERROR_MSG] = error;
  }
    
  if ( triggerEvent.isValid() && triggerResults.isValid() )
  {
    IgDataStorage* storage = config->storage();

    std::string tevent = "TriggerEvent "
                         + edm::TypeID (typeid (TriggerEvent)).friendlyClassName() + ":"
                         + triggerEventTag_.label() + ":"
                         + triggerEventTag_.instance() + ":"
                         + triggerEventTag_.process();
        
    std::string tresult = "TriggerResults "
                          + edm::TypeID (typeid (TriggerResults)).friendlyClassName() + ":"
                          + triggerResultsTag_.label() + ":"
                          + triggerResultsTag_.instance() + ":"
                          + triggerResultsTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = tevent+"  "+tresult;

    IgCollection& triggerObjects = storage->getCollection("TriggerObjects_V1");
	
    IgProperty KEY = triggerObjects.addProperty("key", int(0));
    IgProperty ID = triggerObjects.addProperty("id", int(0));
    IgProperty PT = triggerObjects.addProperty("pt", 0.0);
    IgProperty ETA = triggerObjects.addProperty("eta", 0.0);
    IgProperty PHI = triggerObjects.addProperty("phi", 0.0);
    IgProperty MASS = triggerObjects.addProperty("mass", 0.0);

    const size_type nTO = triggerEvent->sizeObjects();
    const TriggerObjectCollection& triggerObjectCollection(triggerEvent->getObjects());
	
    for ( size_type iT = 0; iT != nTO; ++iT )
    {
      IgCollectionItem t = triggerObjects.create();
      
      const TriggerObject& triggerObject(triggerObjectCollection[iT]);
	    
      t[KEY] = static_cast<int>(iT);
      t[ID] = static_cast<int>(triggerObject.id());
      t[PT] = static_cast<double>(triggerObject.pt());
      t[ETA] = static_cast<double>(triggerObject.eta());
      t[PHI] = static_cast<double>(triggerObject.phi());
      t[MASS] = static_cast<double>(triggerObject.mass());
    }
	
    IgCollection& triggerPaths = storage->getCollection("TriggerPaths_V1");

    IgProperty NAME = triggerPaths.addProperty("Name", std::string());
    IgProperty INDEX = triggerPaths.addProperty("Index", int(0));
    IgProperty WR = triggerPaths.addProperty("WasRun", int(0));
    IgProperty AC = triggerPaths.addProperty("Accept", int(0));
    IgProperty ER = triggerPaths.addProperty("Error", int(0));
    IgProperty OBS = triggerPaths.addProperty("Objects", std::string());

    const unsigned int n(hltConfig_.size());
    
    if ( triggerResults->size() != hltConfig_.size() )
    {   
      std::string error = "### Error: ISpyTriggerEvent: TriggerResults.size() != HLTConfigProvider.size()";
      IgDataStorage* storage = config->storage();
      IgCollection& collection = storage->getCollection("Errors_V1");
      IgProperty ERROR_MSG = collection.addProperty("Error", std::string());
      IgCollectionItem item = collection.create();
      item[ERROR_MSG] = error;

      return;
    }
    
    for ( unsigned int i = 0; i != n; ++i )
    {
      IgCollectionItem tp = triggerPaths.create();
      
      std::string triggerName = hltConfig_.triggerName(i); 
      unsigned int triggerIndex = hltConfig_.triggerIndex(triggerName);
      
      if ( triggerIndex >= n )
      {
        std::string error = "### Error: ISpyTriggerEvent: "+triggerName+" not found";
        IgDataStorage* storage = config->storage();
        IgCollection& collection = storage->getCollection("Errors_V1");
        IgProperty ERROR_MSG = collection.addProperty("Error", std::string());
        IgCollectionItem item = collection.create();
        item[ERROR_MSG] = error;
       
        return;
      }
      
      tp[NAME] = triggerName;
      tp[INDEX] = triggerIndex;
      
      tp[WR] = static_cast<int>(triggerResults->wasrun(triggerIndex));
      tp[AC] = static_cast<int>(triggerResults->accept(triggerIndex));
      tp[ER] = static_cast<int>(triggerResults->error(triggerIndex));

      const std::vector<std::string>& moduleLabels(hltConfig_.moduleLabels(triggerIndex));
      const unsigned int moduleIndex(triggerResults->index(triggerIndex));  

      std::stringstream ss;

      for ( unsigned int j = 0; j <= moduleIndex; ++j ) 
      {
        const std::string&  moduleLabel(moduleLabels[j]);
        const std::string  moduleType(hltConfig_.moduleType(moduleLabel));

        const unsigned int filterIndex(triggerEvent->filterIndex(InputTag(moduleLabel,"",processName_)));

        if ( filterIndex < triggerEvent->sizeFilters() ) 
        {
          const Vids& VIDS = triggerEvent->filterIds(filterIndex);
          const Keys& KEYS = triggerEvent->filterKeys(filterIndex);

          const size_type nI = VIDS.size();
          const size_type nK = KEYS.size();
        
          assert(nI==nK);
          const size_type N(std::max(nI,nK));

          for ( size_type k = 0; k != N; ++k ) 
            ss << VIDS[k] << "/" << KEYS[k] <<" ";
        }
      }
      
      tp[OBS] = ss.str();

    }
  }    
}

DEFINE_FWK_MODULE(ISpyTriggerEvent);
