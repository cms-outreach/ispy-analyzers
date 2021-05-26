import FWCore.ParameterSet.Config as cms

ISpyPATMuon_miniAOD = cms.EDAnalyzer('ISpyPATMuon_miniAOD',
                                     iSpyPATMuonTag = cms.InputTag("allLayer1Muons"),
                                     )
