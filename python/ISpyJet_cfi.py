import FWCore.ParameterSet.Config as cms

ISpyJet = cms.EDAnalyzer('ISpyJet' ,
                         iSpyJetTag = cms.InputTag("iterativeCone5CaloJets")
                         )
