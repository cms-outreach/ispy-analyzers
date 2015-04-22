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
#include "FWCore/ServiceRegistry/interface/StreamContext.h"
#include "FWCore/Version/interface/GetReleaseVersion.h"

#include <iostream>
#include <cstdio>
#include <sstream>

#include "boost/date_time/posix_time/posix_time.hpp"

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

  iRegistry.watchPreEvent(this,&ISpyService::preEvent);
  iRegistry.watchPostEvent(this,&ISpyService::postEvent);

  makeHeader();
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
  
  // Do we want to open one all the time?
  open(outputESFileName_, zipFile1_);
}

void
ISpyService::makeHeader()
{
  // What else do we want to add here?
  // - Provenance?
  // - ScheduleInfo?
  // - versions of Analyzers and Services?
  // - dump of input cfg file?
 
  header_.append(edm::getReleaseVersion());
}

void 
ISpyService::writeHeader(zipFile& zfile)
{
  std::string hs("Header");
  
  zip_fileinfo zi;

  time_t now = time(0);
  boost::posix_time::ptime bt = boost::posix_time::from_time_t(now);
  tm tt = boost::posix_time::to_tm(bt);
                                                                                                         
  zi.tmz_date.tm_sec  = tt.tm_sec;
  zi.tmz_date.tm_min  = tt.tm_min;
  zi.tmz_date.tm_hour = tt.tm_hour;
  zi.tmz_date.tm_mday = tt.tm_mday;
  zi.tmz_date.tm_mon  = tt.tm_mon;
  zi.tmz_date.tm_year = tt.tm_year;
  zi.dosDate = 0;
  zi.internal_fa = 0;
  zi.external_fa = 0;

  ziperr_ = zipOpenNewFileInZip(zfile, hs.c_str(), &zi,
				0, 0, 0, 0, 0, // other stuff                                                                         
				Z_DEFLATED, // method                                                                                 
				9);
  assert(ziperr_ == ZIP_OK);

  std::stringstream doss;
  doss << header_; 
  write(doss, zfile);

  ziperr_ = zipCloseFileInZip(zfile);
  assert(ziperr_ == ZIP_OK);
}

void
ISpyService::open(const std::string &outputFileName, zipFile& zf)
{
  zf = zipOpen(outputFileName.c_str(), APPEND_STATUS_CREATE);  
  writeHeader(zf);
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
ISpyService::preEvent(const edm::StreamContext& sc)
{
  currentRun_   = sc.eventID().run();
  currentEvent_ = sc.eventID().event();

  storages_[0] = new IgDataStorage;
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
ISpyService::postEvent(const edm::StreamContext& sc)
{    
  if ( ! storages_[0]->empty() )
  {
    std::stringstream eoss;
    eoss << "Events/Run_" << currentRun_ << "/Event_" << currentEvent_;

    zip_fileinfo zi;

    time_t now = time(0);
    boost::posix_time::ptime bt = boost::posix_time::from_time_t(now);
    tm tt = boost::posix_time::to_tm(bt);
                                                                                                         
    zi.tmz_date.tm_sec  = tt.tm_sec;
    zi.tmz_date.tm_min  = tt.tm_min;
    zi.tmz_date.tm_hour = tt.tm_hour;
    zi.tmz_date.tm_mday = tt.tm_mday;
    zi.tmz_date.tm_mon  = tt.tm_mon;
    zi.tmz_date.tm_year = tt.tm_year;
    zi.dosDate = 0;
    zi.internal_fa = 0;
    zi.external_fa = 0;
    
    ziperr_ = zipOpenNewFileInZip(zipFile0_, eoss.str().c_str(), &zi,
                                    0, 0, 0, 0, 0, // other stuff
                                    Z_DEFLATED, // method
                                    9);
    assert(ziperr_ == ZIP_OK);

    if ( outputMaxEvents_ != -1 )       
      eventCounter_++;
      
    std::stringstream doss;
    doss << *storages_[0];
    write(doss, zipFile0_);
    
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

    zip_fileinfo zi;

    time_t now = time(0);
    boost::posix_time::ptime bt = boost::posix_time::from_time_t(now);
    tm tt = boost::posix_time::to_tm(bt);
                                                                                                         
    zi.tmz_date.tm_sec  = tt.tm_sec;
    zi.tmz_date.tm_min  = tt.tm_min;
    zi.tmz_date.tm_hour = tt.tm_hour;
    zi.tmz_date.tm_mday = tt.tm_mday;
    zi.tmz_date.tm_mon  = tt.tm_mon;
    zi.tmz_date.tm_year = tt.tm_year;   
    zi.dosDate = 0;
    zi.internal_fa = 0;
    zi.external_fa = 0;
    
    ziperr_ = zipOpenNewFileInZip(zipFile1_, goss.str().c_str(), &zi,
                                    0, 0, 0, 0, 0, 
                                    Z_DEFLATED, 9);
    assert(ziperr_ == ZIP_OK);

    std::stringstream doss;
    doss << *storages_[1];
    write(doss, zipFile1_);

    ziperr_ = zipCloseFileInZip(zipFile1_);
    assert(ziperr_ == ZIP_OK);
  }
  
  delete storages_[1];
  storages_[1] = 0;
}
	
void 
ISpyService::write(std::stringstream& soss, zipFile& zfile)
{
  long int size_buf = soss.str().length();
  void* buf = (void*) malloc(size_buf);
  memcpy((void*) buf, soss.str().c_str(), size_buf);

  ziperr_ = zipWriteInFileInZip(zfile, buf, size_buf);
  
  free(buf);
}

void
ISpyService::error(const std::string & what)
{
  assert (storages_[0]);
  IgCollection& collection = storages_[0]->getCollection("Errors_V1");
  IgProperty ERROR_MSG = collection.addProperty("Error", std::string());
  IgCollectionItem item = collection.create();
  item[ERROR_MSG] = what;
}

DEFINE_FWK_SERVICE(ISpyService);
