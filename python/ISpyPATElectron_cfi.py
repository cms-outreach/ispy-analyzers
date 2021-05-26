import FWCore.ParameterSet.Config as cms

ISpyPATElectron = cms.EDAnalyzer('ISpyPATElectron',
                                 iSpyPATElectronTag = cms.InputTag('allLayer1Electrons')
                                 )
