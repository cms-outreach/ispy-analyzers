#ifndef ANALYZER_ISPY_SERVICE_H
# define ANALYZER_ISPY_SERVICE_H

# include "ISpy/Services/interface/IgNet.h"
# include <deque>
# include <string>

namespace lat 
{
  class OutputStream;
  class ZipArchive;
  class ZipMember;
}

class IgDataStorage;
class ISpyNetProducer;

namespace edm {
  class ActivityRegistry;
  class ParameterSet;
  class EventID;
  class Timestamp;
  class Event;
  class EventSetup;
    
  namespace service {
    class ISpyService
    {
    public:
      ISpyService (const edm::ParameterSet& pSet, ActivityRegistry& iRegistry);

      void 		postBeginJob (void);
      void 		postEndJob (void);
      void 		preEventProcessing (const edm::EventID&, const edm::Timestamp&);
      void 		postEventProcessing (const edm::Event&, const edm::EventSetup&);
      long int		write (const char *data, lat::OutputStream *to, long int maxSize);
      IgDataStorage * 	storage (void) { return storages_[0]; }
      IgDataStorage * 	esStorage (void) { return storages_[1]; }

    private:
      void		init (void);
      lat::ZipArchive *	archive (const std::string & name);
      void		report (void);
      void		registry (void);
      void		produceEvent (const edm::Event & event, const std::string & name, const char *data, long int length);
      std::string	tempFileName (const std::string & name);
      bool		isOnline (void) { return online_; }
      void		finalize (const std::string & name);
	    
      const std::string outputFileName_;
      const std::string outputESFileName_;
      const std::string tmpExt_;
      const std::string ext_;
      int		outputMaxEvents_;
      int		outputMaxTime_;
      bool		outputReport_;	    
      bool		outputRegistry_;	    
      int		eventCounter_;	    
      int		fileCounter_;	    
      int		currentRun_;	    
      int		currentEvent_;
      lat::Time		nextTime_;
      
      std::string	outputHost_;
      unsigned int	outputPort_;
      bool		online_;	    
      bool		debug_;
	    
      lat::ZipArchive 	*archives_[2];
      lat::ZipMember	*current_[2];
      lat::OutputStream	*output_[2];
      IgDataStorage 	*storages_[2];
      std::string	currentFile_[2];	    
      unsigned int	bufferSize_;
      
      ISpyNetProducer	*netProducer_;
      std::deque<std::string> netEvents_;
    };
  }
}

#endif // ANALYZER_ISPY_SERVICE_H
