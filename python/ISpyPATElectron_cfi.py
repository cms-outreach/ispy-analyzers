import FWCore.ParameterSet.Config as cms

ISpyPATElectron = cms.EDAnalyzer('ISpyPATElectron',
                                 iSpyPATElectronTag = cms.InputTag('allLayer1Electrons'),
                                 isAOD = cms.untracked.bool(False)
                                 )
