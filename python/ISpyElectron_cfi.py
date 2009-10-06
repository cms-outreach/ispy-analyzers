import FWCore.ParameterSet.Config as cms

ISpyElectron = cms.EDAnalyzer('ISpyElectron',
                              iSpyElectronTag = cms.InputTag('hltPixelMatchElectronsL1Iso')
                              )
