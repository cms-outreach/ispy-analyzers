#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Analyzers/interface/ISpyNetProducer.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/Provenance/interface/EventID.h"
#include "DataFormats/Provenance/interface/Provenance.h"
#include "DataFormats/Provenance/interface/Timestamp.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ConstProductRegistry.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"
#include "FWCore/ServiceRegistry/interface/ActivityRegistry.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "classlib/iobase/IOFlags.h"
#include "classlib/iobase/File.h"
#include "classlib/iobase/Filename.h"
#include "classlib/iotools/OutputStream.h"
#include "classlib/utils/ShellEnvironment.h"
#include "classlib/zip/ZipArchive.h"
#include "classlib/zip/ZipMember.h"

#include <iostream>
#include <cstdio>

using namespace edm::service;
using namespace lat;

ISpyService::ISpyService (const ParameterSet& iPSet, ActivityRegistry& iRegistry)
  : outputFileName_(iPSet.getUntrackedParameter<std::string>( "outputFileName", std::string("default.ig"))),
    outputESFileName_(iPSet.getUntrackedParameter<std::string>( "outputESFileName", std::string("defaultES.ig"))),
    tmpExt_ (iPSet.getUntrackedParameter<std::string>( "tmpFileExtension", std::string(".tmp"))),
    ext_ (iPSet.getUntrackedParameter<std::string>( "outputFileExtension", std::string(".ig"))),
    outputMaxEvents_(iPSet.getUntrackedParameter<int>( "outputMaxEvents", -1)),
    outputMaxTime_(iPSet.getUntrackedParameter<int>( "outputMaxTime", 3600)),
    outputIg_(iPSet.getUntrackedParameter<bool>( "outputIg", true)),
    outputReport_(iPSet.getUntrackedParameter<bool>( "outputReport", false)),
    outputRegistry_(iPSet.getUntrackedParameter<bool>( "outputRegistry", true)),
    eventCounter_ (0),
    fileCounter_ (1),
    currentRun_ (-1),
    outputHost_(iPSet.getUntrackedParameter<std::string>( "outputHost", std::string("localhost"))),
    outputPort_(iPSet.getUntrackedParameter<unsigned int>( "outputPort", 9000)),
    online_(iPSet.getUntrackedParameter<bool>( "online", false)),
    debug_(iPSet.getUntrackedParameter<bool>( "debug", false)),
    bufferSize_(iPSet.getUntrackedParameter<unsigned int>( "bufferSize", 10)),
    netProducer_(0)
{
  archives_[0] = archives_[1] = 0;

  iRegistry.watchPostBeginJob(this,&ISpyService::postBeginJob);
  iRegistry.watchPostEndJob(this,&ISpyService::postEndJob);

  iRegistry.watchPreProcessEvent(this,&ISpyService::preEventProcessing);
  iRegistry.watchPostProcessEvent(this,&ISpyService::postEventProcessing);

  init ();
}

void
ISpyService::init (void)
{
  if (isOnline ())
  {	
    netProducer_ = new ISpyNetProducer (debug_, outputPort_);
    netProducer_->start ();
  }
}

void
ISpyService::postBeginJob (void)
{
  if (outputIg_)
  {
    currentFile_[0] = tempFileName (outputFileName_);
    archives_[0] = archive (currentFile_[0]);
    if (outputRegistry_) registry ();
    currentFile_[1] = tempFileName (outputESFileName_);
    archives_[1] = archive (currentFile_[1]);
  }
  
  nextTime_ = Time::current () + TimeSpan (0, 0, 0, outputMaxTime_ /* seconds */, 0);
}

lat::ZipArchive*
ISpyService::archive (const std::string &outputFileName) 
{
  Filename zipOutputFile (Filename (outputFileName).substitute (ShellEnvironment ()));
    
  return new ZipArchive (zipOutputFile, IOFlags::OpenWrite
			 | IOFlags::OpenCreate | IOFlags::OpenTruncate);
}

std::string
ISpyService::tempFileName (const std::string &fileName)
{
  std::string tmpFileName (fileName);
  size_t found;    
  found = tmpFileName.rfind (ext_);
  if (found != std::string::npos)
    tmpFileName.replace (found, ext_.length (), tmpExt_);
  else
    tmpFileName.append (tmpExt_);
    
  return tmpFileName;
}

void
ISpyService::postEndJob (void)
{
  if (outputIg_)
  {
    if (outputReport_) report ();
    if (! archives_[0] == 0 && archives_[0]->size () > 0)
    {	    
      archives_[0]->close ();
      delete archives_[0];
      archives_[0] = 0;
      finalize (currentFile_[0]);
    }
    
    if (! archives_[1] == 0 && archives_[1]->size () > 0)
    {	    
      archives_[1]->close ();
      delete archives_[1];
      archives_[1] = 0;
      finalize (currentFile_[1]);
    }
  }
  
  if (netProducer_)
  {	
    netProducer_->lock ();
    netEvents_.clear ();
    netProducer_->unlock ();
    netProducer_->sendLocalChanges ();	
    interrupt (9);
    netProducer_->shutdown ();
	
    delete netProducer_;	
  }
}

void
ISpyService::finalize (const std::string & name)
{
  std::string fromFileName = name;
  Filename fromFile (Filename (fromFileName).substitute (ShellEnvironment ()));

  if (fromFile.exists () && (! fromFile.empty ()))
  {
    std::string toFileName = fromFileName;
    size_t found;
    found = toFileName.find (tmpExt_);

    if (found != std::string::npos)
      toFileName.erase (toFileName.end () - tmpExt_.length (), toFileName.end ());
    
    char suffix[64];
    sprintf (suffix, "_R%09d_T%08d.ig", currentRun_, fileCounter_);
    toFileName.append (suffix);
    
    Filename toFile (Filename (toFileName).substitute (ShellEnvironment ()));
    std::rename (fromFile.name (), toFile.name ());
  }
}

void
ISpyService::preEventProcessing (const edm::EventID& event, const edm::Timestamp& timestamp)
{
  currentRun_ = event.run();
  currentEvent_ = event.event();
    
  if (outputIg_ && archives_[0] == 0)
  {
    currentFile_[0] = tempFileName (outputFileName_);
    archives_[0] = archive (currentFile_[0]);
  }

  storages_[0] = new IgDataStorage;
  storages_[1] = new IgDataStorage;
}

void
ISpyService::postEventProcessing (const edm::Event& event, const edm::EventSetup& eventSetup)
{    
  if (!storages_[0]->empty())
  {
    std::stringstream oss;
    oss << *storages_[0];
    if (isOnline ())
    {
      ASSERT (netProducer_);	

      std::stringstream noss;
      noss << "Events/Run_" << event.run () << "/Event_" << event.id ().event ();

      produceEvent (event, noss.str (), oss.str ().c_str (), oss.str ().length ());	
    }
    if (outputIg_)
    {      
      std::stringstream eoss;
      eoss << "Events/Run_" << currentRun_ << "/Event_" << currentEvent_;
	
      current_[0] = new ZipMember (eoss.str ());
      current_[0]->isDirectory (false);
      current_[0]->time (Time::current ());
      current_[0]->method (ZConstants::DEFLATED);
      current_[0]->level (ZConstants::BEST_COMPRESSION);
    
      output_[0] = archives_[0]->output (current_[0]);
      char str[] = "\n";
      output_[0]->write (str, sizeof (str)-1);
    
      if (outputMaxEvents_ != -1)
	eventCounter_++;
    
      ASSERT (output_[0]);

      write (oss.str ().c_str (), output_[0], oss.str ().length ());
    
      output_[0]->close ();
      delete output_[0];
      output_[0] = 0;
    }
    delete storages_[0];    
    storages_[0] = 0;
  
    if (! storages_[1]->empty ())
    {	
      std::stringstream goss;
      goss << "Geometry/Run_" << event.run () << "/Event_" << event.id ().event ();
    
      current_[1] = new ZipMember (goss.str ());
      current_[1]->isDirectory (false);
      current_[1]->time (Time::current ());
      current_[1]->method (ZConstants::DEFLATED);
      current_[1]->level (ZConstants::BEST_COMPRESSION);

      ASSERT (archives_[1]);	
      output_[1] = archives_[1]->output (current_[1]);

      ASSERT (output_[1]);

      std::stringstream ossES;
      ossES << *storages_[1];
      write (ossES.str ().c_str (),  output_[1], ossES.str ().length ());
      output_[1]->close ();
      delete output_[1];
      output_[1] = 0;
    }
    delete storages_[1];    
    storages_[1] = 0;
    
    if (eventCounter_ == outputMaxEvents_
	|| Time::current () > nextTime_)
    {
      archives_[0]->close ();
      delete archives_[0];
      archives_[0] = 0;
      finalize (currentFile_[0]);	
      eventCounter_ = 0;
      fileCounter_++;
      nextTime_ = Time::current () + TimeSpan (0, 0, 0, outputMaxTime_ /* seconds */, 0);
    }
  }
}
	    
long int
ISpyService::write (const char *data, lat::OutputStream *to, long int maxSize) 
{
  ASSERT (to);
  to->write (data, maxSize);
  return maxSize;
}

void
ISpyService::error (const std::string & what)
{
  ASSERT (storages_[0]);
  IgCollection& collection = storages_[0]->getCollection("Errors_V1");
  IgProperty ERROR_MSG = collection.addProperty("Error", std::string());
  IgCollectionItem item = collection.create();
  item[ERROR_MSG] = what;
}

void
ISpyService::report (void) 
{
  std::string footerName = "Footer";
    
  ZipMember *footer = new ZipMember (footerName);
  footer->isDirectory (false);
  footer->time (Time::current ());
  footer->method (ZConstants::DEFLATED);
  footer->level (ZConstants::BEST_COMPRESSION);
    
  OutputStream *output = archives_[0]->output (footer);
  char str[] = "ISpyService::postEndJob\n";
  output->write (str, sizeof (str)-1);
  output->close ();
  delete output;
}

void
ISpyService::registry (void) 
{    
  // FIXME: Do it once per run?
  outputRegistry_ = false;
    
  std::string headerName = "Header";
    
  ZipMember *header = new ZipMember (headerName);
  header->isDirectory (false);
  header->time (Time::current ());
  header->method (ZConstants::DEFLATED);
  header->level (ZConstants::BEST_COMPRESSION);
    
  ASSERT (archives_[0]);
    
  OutputStream *output = archives_[0]->output (header);

  edm::Service<edm::ConstProductRegistry> reg;

  std::string friendlyName;
  std::string moduleLabel;
  std::string instanceName;
  std::string processName;

  // Loop over provenance of products in registry.
  for (edm::ProductRegistry::ProductList::const_iterator it = reg->productList ().begin ();
       it != reg->productList ().end (); ++it) 
  {
    // See FWCore/Framework/interface/BranchDescription.h
    // BranchDescription contains all the information for the product.
    edm::BranchDescription desc = it->second;
    friendlyName = desc.friendlyClassName ();
    moduleLabel = desc.moduleLabel ();
    instanceName = desc.productInstanceName ();
    processName = desc.processName ();
	
    std::string twigName = friendlyName
			   + std::string (":") + moduleLabel
			   + std::string (":") + instanceName 
			   + std::string (":") + processName + std::string ("\n");
    output->write (twigName.c_str (), twigName.length ());
  }
 
  output->close ();
  delete output;
}

void
ISpyService::produceEvent(const edm::Event& event, const std::string & name, const char *data, long int length)
{
  if(netEvents_.size() > bufferSize_)
  {	    
    netProducer_->lock();
    for(std::deque<std::string>::iterator it = netEvents_.begin(), itEnd = netEvents_.end();
	it != itEnd; ++it)
    {
      netProducer_->removeLocalObject(*it);	    
      netEvents_.pop_front();
    }    
    netProducer_->unlock();
    netProducer_->sendLocalChanges();
  }

  IgNet::Object o;
  o.version = Time::current().ns();
  o.flags = IgNet::VIS_FLAG_NEW | IgNet::VIS_FLAG_SCALAR;
  o.name = name;
  o.rawdata.resize(length);
  memcpy(&o.rawdata[0], &data[0], length);
  o.lastreq = 0;
  netEvents_.push_back(name);
  
  netProducer_->lock();
  netProducer_->updateLocalObject(o);
  netProducer_->unlock();
  netProducer_->sendLocalChanges();
}

DEFINE_FWK_SERVICE(ISpyService);
