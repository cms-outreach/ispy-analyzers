import FWCore.ParameterSet.Config as cms

ISpyPFJet = cms.EDAnalyzer('ISpyPFJet',
                            iSpyPFJetTag = cms.InputTag('iterativeCone5PFJets')
                           )
