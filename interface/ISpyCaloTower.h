#ifndef ANALYZER_ISPY_CALO_TOWER_H
# define ANALYZER_ISPY_CALO_TOWER_H

# include "FWCore/Framework/interface/EDAnalyzer.h"

class ISpyCaloTower : public edm::EDAnalyzer
{
public:
  explicit ISpyCaloTower(const edm::ParameterSet&);
  virtual ~ISpyCaloTower(void) {}
       
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag	inputTag_;
};

#endif // ANALYZER_ISPY_CALO_TOWER_H
