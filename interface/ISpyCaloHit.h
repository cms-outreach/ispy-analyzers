#ifndef ANALYZER_ISPY_CALOHIT_H
#define ANALYZER_ISPY_CALOHIT_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

typedef std::vector<edm::InputTag> VInputTag;

class ISpyCaloHit : public edm::EDAnalyzer
{
public:
  explicit ISpyCaloHit(const edm::ParameterSet&);
  virtual ~ISpyCaloHit(void){}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag	inputTag_;
  VInputTag	inputTags_;
};

#endif // ANALYZER_ISPY_CALOHIT_H
