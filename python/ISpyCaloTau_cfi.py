import FWCore.ParameterSet.Config as cms

ISpyCaloTau = cms.EDAnalyzer('ISpyCaloTau',
                            iSpyCaloTauTag = cms.InputTag('caloRecoTauProducer')
                           )
