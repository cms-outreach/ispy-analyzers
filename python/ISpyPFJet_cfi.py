import FWCore.ParameterSet.Config as cms

ISpyPFJet = cms.EDAnalyzer('ISpyPFJet',
                            iSpyPFJetTag = cms.InputTag('iterativeCone5PFJets'),
                            etMin = cms.double(1.0),
                            etaMax = cms.double(2.5)
                           )
