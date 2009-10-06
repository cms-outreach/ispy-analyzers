import FWCore.ParameterSet.Config as cms

ISpyMET = cms.EDAnalyzer('ISpyMET' ,
                         iSpyMETTag = cms.InputTag("htMetIC5")
                         )
