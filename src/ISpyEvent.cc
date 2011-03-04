#include "ISpy/Analyzers/interface/ISpyEvent.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "boost/date_time/posix_time/posix_time.hpp"
#include <sys/time.h> 

using namespace edm::service;

namespace {
  std::string
    getLocalTime(const edm::Event& event)
  {
    time_t t(event.time().value() >> 32);
    std::string text(asctime(localtime(&t)));
    size_t pos = text.find('\n');
    if (pos != std::string::npos) text = text.substr(0, pos);
    text += " ";
    if (daylight)
      text += tzname[1];
    else
      text += tzname[0];
    return text;
  }
}

ISpyEvent::ISpyEvent (const edm::ParameterSet& iPSet)
{}

void 
ISpyEvent::analyze( const edm::Event& event, const edm::EventSetup& /* eventSetup */)
{
  edm::Service<ISpyService> config;
  if (! config.isAvailable ()) 
  {
    throw cms::Exception ("Configuration")
      << "ISpyEvent requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }
  const edm::Timestamp time = event.time ();

  timeval eventTime;
  eventTime.tv_sec = time.value () >> 32;
  eventTime.tv_usec = 0xFFFFFFFF & time.value ();

  boost::posix_time::ptime bt0 = boost::posix_time::from_time_t(0);
  boost::posix_time::ptime bt = bt0 + boost::posix_time::seconds(eventTime.tv_sec)
				+ boost::posix_time::microseconds(eventTime.tv_usec);

  std::stringstream oss;
  oss << bt << " GMT";

  std::string datetime (oss.str ());

  IgDataStorage *storage = config->storage ();
  assert(storage);
  IgCollection &eventColl = storage->getCollection ("Event_V2");

  IgProperty RUN   = eventColl.addProperty ("run", static_cast<int>(0));
  IgProperty EVENT = eventColl.addProperty ("event", static_cast<int>(0));
  IgProperty LS    = eventColl.addProperty ("ls", static_cast<int>(0));
  IgProperty ORBIT = eventColl.addProperty ("orbit", static_cast<int>(0));
  IgProperty BX    = eventColl.addProperty ("bx", static_cast<int>(0));
  IgProperty TIME  = eventColl.addProperty ("time", std::string ());    
  IgProperty LOCALTIME  = eventColl.addProperty ("localtime", std::string ());    

  IgCollectionItem eventId = eventColl.create();
  eventId [RUN]   = static_cast<int>(event.id ().run ());
  eventId [EVENT] = static_cast<int>(event.id ().event ());
  eventId [LS]    = static_cast<int>(event.luminosityBlock ());
  eventId [ORBIT] = static_cast<int>(event.orbitNumber ());
  eventId [BX]    = static_cast<int>(event.bunchCrossing ());
  eventId [TIME]  = static_cast<std::string>(datetime);
  eventId [LOCALTIME]  = static_cast<std::string>(getLocalTime(event));
}

DEFINE_FWK_MODULE(ISpyEvent);
