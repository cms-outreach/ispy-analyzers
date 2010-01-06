import FWCore.ParameterSet.Config as cms

ISpyPFMET = cms.EDAnalyzer('ISpyPFMET',
                            iSpyPFMETTag = cms.InputTag('pfMet')
                           )
