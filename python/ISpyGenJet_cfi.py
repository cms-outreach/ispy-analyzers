import FWCore.ParameterSet.Config as cms

ISpyGenJet = cms.EDAnalyzer('ISpyGenJet' ,
                            iSpyGenJetTag = cms.InputTag("slimmedGenJets")
                            )
