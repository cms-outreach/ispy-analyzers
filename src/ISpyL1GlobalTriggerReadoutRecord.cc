#include "ISpy/Analyzers/interface/ISpyL1GlobalTriggerReadoutRecord.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"

#include "CondFormats/L1TObjects/interface/L1GtTriggerMenu.h"
#include "CondFormats/L1TObjects/interface/L1GtTriggerMask.h"
#include "CondFormats/DataRecord/interface/L1GtTriggerMenuRcd.h"
#include "CondFormats/DataRecord/interface/L1GtTriggerMaskAlgoTrigRcd.h"
#include "CondFormats/DataRecord/interface/L1GtTriggerMaskTechTrigRcd.h"

#include <sstream>

using namespace edm::service;
using namespace edm;

ISpyL1GlobalTriggerReadoutRecord::ISpyL1GlobalTriggerReadoutRecord(const edm::ParameterSet& iConfig)
  : inputTag_(iConfig.getParameter<edm::InputTag>("iSpyL1GlobalTriggerReadoutRecordTag")){}

void ISpyL1GlobalTriggerReadoutRecord::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyL1GlobalTriggerReadoutRecord requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  edm::Handle<L1GlobalTriggerReadoutRecord> triggerRecord;
  event.getByLabel(inputTag_, triggerRecord);

  edm::ESHandle<L1GtTriggerMask> l1GtTmTech;
  edm::ESHandle<L1GtTriggerMask> l1GtTmAlgo;
  edm::ESHandle<L1GtTriggerMenu> menuRcd;

  if( triggerRecord.isValid() )
  {
    IgDataStorage* storage = config->storage();

    std::string product = "L1GlobalTriggerReadoutRecord "
			  + edm::TypeID (typeid (L1GlobalTriggerReadoutRecord)).friendlyClassName() + ":"
			  + inputTag_.label() + ":"
			  + inputTag_.instance() + ":"
			  + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection& gtTrigger = storage->getCollection("L1GtTrigger_V1");
	
    IgProperty ALGO = gtTrigger.addProperty("algorithm", std::string());
    IgProperty ABN  = gtTrigger.addProperty("algoBitNumber", int(0));
    IgProperty RES  = gtTrigger.addProperty("result", int(0));

    edm::ESHandle<L1GtTriggerMenu> menuRcd;
    eventSetup.get<L1GtTriggerMenuRcd>().get(menuRcd) ;

    const L1GtTriggerMenu* menu = menuRcd.product();
    const AlgorithmMap amap = menu->gtAlgorithmMap();

    const DecisionWord dWord = triggerRecord->decisionWord();
 
    for( AlgorithmMap::const_iterator mit = amap.begin(), mitEnd = amap.end(); 
	 mit != mitEnd; ++mit )
    {
      std::string algo = mit->first;
      int result = menu->gtAlgorithmResult(algo, dWord);

      IgCollectionItem t = gtTrigger.create();
      
      int algoBitNumber = (mit->second).algoBitNumber();
            
      t[ALGO] = algo;
      t[ABN]  = algoBitNumber;
      t[RES]  = result;
    }

    /*
      What to do with these?

      eventSetup.get< L1GtTriggerMaskTechTrigRcd >().get (l1GtTmTech); 
      const L1GtTriggerMask *trigMask = l1GtTmTech.product ();

      eventSetup.get< L1GtTriggerMaskAlgoTrigRcd >().get (l1GtTmAlgo);        
      const L1GtTriggerMask *trigMaskAlgo = l1GtTmAlgo.product ();
    */

    const TechnicalTriggerWord gtTTWord = triggerRecord->technicalTriggerWord();

    IgCollection& tTrigger = storage->getCollection("TechTrigger_V1");
	
    IgProperty TBIT = tTrigger.addProperty("bitNumber", int(0));
    IgProperty TRESULT = tTrigger.addProperty("result", int(0));

    int tbitNumber = 0;
    TechnicalTriggerWord::const_iterator GTtbitItr;
    for(GTtbitItr = gtTTWord.begin(); GTtbitItr != gtTTWord.end(); GTtbitItr++)
    {
      IgCollectionItem tt = tTrigger.create();
      tt[TBIT]  = tbitNumber;
      if(*GTtbitItr)
	tt[TRESULT]  = 1;
      else
	tt[TRESULT]  = 0;
	
      tbitNumber++; 
    }
	
    /*
      Are these useful?

      IgCollection& l1GtfeWordColl  = storage->getCollection("L1GtfeWord_V1");
	
      IgProperty BID  = l1GtfeWordColl.addProperty("BoardId", std::string());
      IgProperty BX1  = l1GtfeWordColl.addProperty("BxAlt1", std::string());
      IgProperty BX0  = l1GtfeWordColl.addProperty("BxAlt0", std::string());
      IgProperty BXNR = l1GtfeWordColl.addProperty("BxNr", std::string());
      IgProperty SV   = l1GtfeWordColl.addProperty("SetupVersion", std::string());
      IgProperty AB   = l1GtfeWordColl.addProperty("ActiveBoards", std::string());
      IgProperty ANBB = l1GtfeWordColl.addProperty("AltNrBxBoard", std::string());
      IgProperty TTN  = l1GtfeWordColl.addProperty("TotalTriggerNr", std::string());

      const L1GtfeWord& l1GtfeWord = triggerRecord->gtfeWord();
	
      IgCollectionItem gtfe = l1GtfeWordColl.create();
	
      std::ostringstream bid;
      bid <<"x"<< std::hex << l1GtfeWord.boardId();
      gtfe[BID] = std::string(bid.str());
	
      std::ostringstream rl1;
      rl1 <<"x"<< std::hex << l1GtfeWord.recordLength1();
      gtfe[BX1] = std::string(rl1.str());
	
      std::ostringstream rl0;
      rl0 <<"x"<< std::hex << l1GtfeWord.recordLength1();
      gtfe[BX0] = std::string(rl0.str());
	
      std::ostringstream bxnr;
      bxnr <<"x"<< std::hex << l1GtfeWord.bxNr();
      gtfe[BXNR] = std::string(bxnr.str());
	
      std::ostringstream sv;
      sv <<"x"<< std::hex << l1GtfeWord.setupVersion();
      gtfe[SV] = std::string(sv.str());

      std::ostringstream ab;
      ab <<"x"<< std::hex << l1GtfeWord.activeBoards();
      gtfe[AB] = std::string(ab.str());

      std::ostringstream anbb;
      anbb <<"x"<< std::hex << l1GtfeWord.altNrBxBoard();
      gtfe[ANBB] = std::string(anbb.str());

      std::ostringstream ttn;
      ttn <<"x"<< std::hex << l1GtfeWord.totalTriggerNr();
      gtfe[TTN] = std::string(ttn.str());
		
      IgCollection& l1GtFdlWordColl = storage->getCollection("L1GtFdlWord_V1");
      IgCollection& l1GtPsbWordColl = storage->getCollection("L1GtPsbWord_V1");

      for ( std::vector<L1GtFdlWord>::const_iterator itFdl = 
      triggerRecord->gtFdlVector().begin();
      itFdl != triggerRecord->gtFdlVector().end(); ++itFdl ) 
      {
      itFdl->print(std::cout);	 
      }
 
      for ( std::vector<L1GtPsbWord>::const_iterator itPsb = 
      triggerRecord->gtPsbVector().begin();	
      itPsb != triggerRecord->gtPsbVector().end(); ++itPsb ) 
      {
	     
      itPsb->print(std::cout);
      }
    */
  }

  else
  {
    std::string error = "### Error: L1GlobalTriggerReadoutRecord "

			+ edm::TypeID (typeid (L1GlobalTriggerReadoutRecord)).friendlyClassName() + ":"
			+ inputTag_.label() + ":"
			+ inputTag_.instance() + ":"
			+ inputTag_.process() + " are not found.";

    IgDataStorage* storage = config->storage();
    IgCollection& collection = storage->getCollection("Errors_V1");
    IgProperty ERROR_MSG = collection.addProperty("Error", std::string());
    IgCollectionItem item = collection.create();
    item[ERROR_MSG] = error;
  }

}

DEFINE_FWK_MODULE(ISpyL1GlobalTriggerReadoutRecord);
