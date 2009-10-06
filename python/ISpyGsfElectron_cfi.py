import FWCore.ParameterSet.Config as cms

ISpyGsfElectron = cms.EDAnalyzer('ISpyGsfElectron',
                                 iSpyGsfElectronTag = cms.InputTag('gsfElectrons')
                                 )
