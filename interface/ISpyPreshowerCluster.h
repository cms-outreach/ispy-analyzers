#ifndef ANALYZER_ISPY_PRESHOWERCLUSTER_H
#define ANALYZER_ISPY_PRESHOWERCLUSTER_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include <vector>

typedef std::vector<edm::InputTag> VInputTag;

class ISpyPreshowerCluster : public edm::one::EDAnalyzer<>
{
public:
    explicit ISpyPreshowerCluster(const edm::ParameterSet&);
    virtual ~ISpyPreshowerCluster(){};
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
    VInputTag inputTags_;
};
#endif
