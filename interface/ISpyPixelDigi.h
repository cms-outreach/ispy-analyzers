#ifndef ANALYZER_ISPY_PIXEL_DIGI_H
#define ANALYZER_ISPY_PIXEL_DIGI_H

#include "FWCore/Framework/interface/EDAnalyzer.h"

class ISpyPixelDigi : public edm::EDAnalyzer
{
public:
  explicit ISpyPixelDigi (const edm::ParameterSet&);
  virtual ~ISpyPixelDigi (void) {}
  
  virtual void analyze( const edm::Event&, const edm::EventSetup& );
private:
  edm::InputTag	inputTag_;
};

#endif // ANALYZER_ISPY_PIXEL_DIGI_H
