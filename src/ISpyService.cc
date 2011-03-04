#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"
#include "ISpy/Services/interface/IgArchive.h"

#include "DataFormats/Provenance/interface/EventID.h"
#include "DataFormats/Provenance/interface/Provenance.h"
#include "DataFormats/Provenance/interface/Timestamp.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ConstProductRegistry.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"
#include "FWCore/ServiceRegistry/interface/ActivityRegistry.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include <iostream>
#include <cstdio>
#include <sstream>

using namespace edm::service;

ISpyService::ISpyService (const ParameterSet& iPSet, ActivityRegistry& iRegistry)
  : outputFileName_(iPSet.getUntrackedParameter<std::string>( "outputFileName", std::string("default.ig"))),
    outputESFileName_(iPSet.getUntrackedParameter<std::string>( "outputESFileName", std::string("defaultES.ig"))),
    fileExt_(std::string(".ig")),
    currentExt_(std::string("")),
    outputMaxEvents_(iPSet.getUntrackedParameter<int>( "outputMaxEvents", -1)),
    eventCounter_(0),
    fileCounter_(0),
    currentRun_(-1),
    currentEvent_(-1)
{
  iRegistry.watchPostBeginJob(this,&ISpyService::postBeginJob);
  iRegistry.watchPostEndJob(this,&ISpyService::postEndJob);

  iRegistry.watchPreProcessEvent(this,&ISpyService::preEventProcessing);
  iRegistry.watchPostProcessEvent(this,&ISpyService::postEventProcessing);
}

void
ISpyService::postBeginJob (void)
{
  // If the input file has an .ig file extension (and it should)
  // then add 0 counter (replacing bare .ig). 
  // If not, then add counter and extension anyway.     
  std::stringstream foss;
  foss << fileCounter_;
  currentExt_.append("_");
  currentExt_.append(foss.str());
  currentExt_.append(fileExt_);
  
  size_t found = outputFileName_.find(fileExt_);

  if ( found != std::string::npos )
    outputFileName_.erase(outputFileName_.end() - fileExt_.length(), outputFileName_.end());
  
  outputFileName_.append(currentExt_);
  open(outputFileName_, zipFile0_);
 
  // Do we always want to do this?
  // Perhaps specify whether or not we want to do this
  // with a cfg parameter?
  open(outputESFileName_, zipFile1_);
}

void
ISpyService::open(const std::string &outputFileName, zipFile& zf)
{
  zf = zipOpen64(outputFileName.c_str(), APPEND_STATUS_CREATE);  
}

void
ISpyService::close(zipFile& zf)
{
  ziperr_ = zipClose(zf, 0);
  assert(ziperr_ == ZIP_OK);
}

void
ISpyService::postEndJob(void)
{
  // If we haven't yet reached the max number of events
  // for each ig file but are at the end of the root input
  // OR we are writing all events to the file 
  // (i.e. no max number set), close zip file which contains events

  if ( (eventCounter_ > 0 && eventCounter_ < outputMaxEvents_) || 
       outputMaxEvents_ == -1 ) 
    close(zipFile0_);
        
  close(zipFile1_);
}

void
ISpyService::preEventProcessing(const edm::EventID& event, const edm::Timestamp& timestamp)
{
  currentRun_   = event.run();
  currentEvent_ = event.event();

  storages_[0] = new IgDataStorage;

  // See related comment in postBeginJob 
  storages_[1] = new IgDataStorage;

  // If an ig file has already been written but we have yet to
  // process an event for the next bunch, then open a new zip file
  if ( fileCounter_ > 0 && eventCounter_ == 0 )
  {
    std::stringstream foss;
    foss << fileCounter_;
    
    size_t found = outputFileName_.find(currentExt_);
    std::string newExt("_"+foss.str()+fileExt_); 

    if ( found != std::string::npos )
      outputFileName_.erase(outputFileName_.end() - currentExt_.length(), outputFileName_.end());
      
    outputFileName_.append(newExt);
    currentExt_ = newExt;
    open(outputFileName_, zipFile0_);
  }
}

void
ISpyService::postEventProcessing(const edm::Event& event, const edm::EventSetup& eventSetup)
{    
  if ( ! storages_[0]->empty() )
  {
    std::stringstream eoss;
    eoss << "Events/Run_" << currentRun_ << "/Event_" << currentEvent_;
	
    zip_fileinfo zi; // Do something with this 
    zi.tmz_date.tm_sec  = 0;
    zi.tmz_date.tm_min  = 0;
    zi.tmz_date.tm_hour = 0;
    zi.tmz_date.tm_mday = 0;
    zi.tmz_date.tm_mon  = 0;
    zi.tmz_date.tm_year = 0;
    zi.dosDate = 0;
    zi.internal_fa = 0;
    zi.external_fa = 0;
    
    ziperr_ = zipOpenNewFileInZip64(zipFile0_, eoss.str().c_str(), &zi,
                                    0, 0, 0, 0, 0, // other stuff
                                    Z_DEFLATED, // method
                                    9, // compression level
                                    0);
    assert(ziperr_ == ZIP_OK);

    if ( outputMaxEvents_ != -1 )       
      eventCounter_++;
      
    write(storages_[0], zipFile0_);
    
    ziperr_ = zipCloseFileInZip(zipFile0_);
    assert(ziperr_ == ZIP_OK);

    // If we are at the maximum number of events
    // for each ig file then we must close the current 
    // zip file and open another one 
    if ( eventCounter_ == outputMaxEvents_ )
    {
      close(zipFile0_);
      fileCounter_ += 1;
      eventCounter_ = 0;
    }
  }

  delete storages_[0];    
  storages_[0] = 0;

  if ( ! storages_[1]->empty() )
  {
    std::stringstream goss;
    goss << "Geometry/Run_"<< currentRun_ <<"/Event_"<< currentEvent_;

    zip_fileinfo zi; // Do something with this
    zi.tmz_date.tm_sec  = 0;
    zi.tmz_date.tm_min  = 0;
    zi.tmz_date.tm_hour = 0;
    zi.tmz_date.tm_mday = 0;
    zi.tmz_date.tm_mon  = 0;
    zi.tmz_date.tm_year = 0;
    zi.dosDate = 0;
    zi.internal_fa = 0;
    zi.external_fa = 0;

    write(storages_[1], zipFile1_);

    ziperr_ = zipOpenNewFileInZip64(zipFile1_, goss.str().c_str(), &zi,
                                    0, 0, 0, 0, 0, 
                                    Z_DEFLATED, 9, 0);
    assert(ziperr_ == ZIP_OK);
  }
  
  delete storages_[1];
  storages_[1] = 0;
}
	
void 
ISpyService::write(IgDataStorage* storage, zipFile& zfile)
{
  std::stringstream doss;
  doss << *storage; 
 
  long int size_buf = sizeof(std::string::size_type) + doss.str().length();
  void* buf = (void*) malloc(size_buf);
  memcpy((void*) buf, doss.str().c_str(), size_buf);

  ziperr_ = zipWriteInFileInZip(zfile, buf, size_buf);
  
  free(buf);
}

void
ISpyService::error (const std::string & what)
{
  assert (storages_[0]);
  IgCollection& collection = storages_[0]->getCollection("Errors_V1");
  IgProperty ERROR_MSG = collection.addProperty("Error", std::string());
  IgCollectionItem item = collection.create();
  item[ERROR_MSG] = what;
}

DEFINE_FWK_SERVICE(ISpyService);
