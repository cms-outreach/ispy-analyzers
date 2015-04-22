#ifndef ANALYZER_ISPY_SERVICE_H
#define ANALYZER_ISPY_SERVICE_H

#include <string>
#include <ISpy/Services/interface/zip.h>

class IgDataStorage;

namespace edm {
  class ActivityRegistry;
  class StreamContext;
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
      void 		preEvent (const edm::StreamContext&);
      void 		postEvent (const edm::StreamContext&);

      IgDataStorage * 	storage (void) { return storages_[0]; }
      IgDataStorage * 	esStorage (void) { return storages_[1]; }
      void		error (const std::string & what);

    private:
      void              open(const std::string& name, zipFile& zfile);
      void              write(std::stringstream& soss, zipFile& zfile);
      void              close(zipFile& zfile);
      void              makeHeader();
      void              writeHeader(zipFile& zfile);
	    
      std::string       outputFileName_;
      std::string       outputESFileName_;
      std::string       fileExt_;
      std::string       currentExt_;
      std::string       header_;

      int		outputMaxEvents_;
      int		eventCounter_;	    
      int		fileCounter_;	    
      int		currentRun_;	    
      int		currentEvent_;
      
      zipFile           zipFile0_; // Events
      zipFile           zipFile1_; // Geometry
      IgDataStorage 	*storages_[2];

      bool              fileWritten_;
      int               ziperr_;
    };
  }
}

#endif // ANALYZER_ISPY_SERVICE_H
