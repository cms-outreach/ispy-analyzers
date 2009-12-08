import FWCore.ParameterSet.Config as cms

ISpyPATJet = cms.EDAnalyzer('ISpyPATJet',
                            iSpyPATJetTag = cms.InputTag('cleanLayer1Jets')
                           )
