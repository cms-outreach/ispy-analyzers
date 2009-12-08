import FWCore.ParameterSet.Config as cms

ISpyPATMET = cms.EDAnalyzer('ISpyPATMET',
                            iSpyPATMETTag = cms.InputTag('layer1METs')
                           )
