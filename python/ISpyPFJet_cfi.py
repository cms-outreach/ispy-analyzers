import FWCore.ParameterSet.Config as cms

ISpyPFJet = cms.EDAnalyzer('ISpyPFJet',
                           iSpyPFJetTag = cms.InputTag('ak4PFJetsCHS'),
                           iSpyAK4PFJetTag = cms.InputTag('ak4PFJets'),
                           iSpyAK8PFJetTag = cms.InputTag('ak8PFJets'),
                           etMin = cms.double(1.0),
                           etaMax = cms.double(2.5)
                           )
