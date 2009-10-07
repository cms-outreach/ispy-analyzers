#ifndef ANALYZER_ISPY_PRESHOWERCLUSTER_H
#define ANALYZER_ISPY_PRESHOWERCLUSTER_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include <vector>

typedef std::vector<edm::InputTag> VInputTag;

class ISpyPreshowerCluster : public edm::EDAnalyzer
{
public:
    explicit ISpyPreshowerCluster(const edm::ParameterSet&);
    virtual ~ISpyPreshowerCluster(){};
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
    VInputTag inputTags_;
};
#endif
