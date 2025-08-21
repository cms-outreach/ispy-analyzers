import FWCore.ParameterSet.Config as cms

ISpyPATJet = cms.EDAnalyzer('ISpyPATJet',
                            iSpyPATJetTag = cms.InputTag('slimmedJets'),
                            iSpyAK4PATJetTag = cms.InputTag('slimmedJetsAK4'),
                            iSpyAK8PATJetTag = cms.InputTag('slimmedJetsAK8')
                           )
