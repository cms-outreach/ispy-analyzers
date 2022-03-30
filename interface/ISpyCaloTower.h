#ifndef ANALYZER_ISPY_CALO_TOWER_H
#define ANALYZER_ISPY_CALO_TOWER_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/CaloTowers/interface/CaloTowerCollection.h"

class ISpyCaloTower : public edm::one::EDAnalyzer<>
{

public:
  explicit ISpyCaloTower(const edm::ParameterSet&);
  virtual ~ISpyCaloTower(void) {}
       
  virtual void analyze(const edm::Event&, const edm::EventSetup&);

private:
  edm::InputTag	inputTag_;
  edm::EDGetTokenT<CaloTowerCollection> caloTowerToken_;

};

#endif // ANALYZER_ISPY_CALO_TOWER_H
