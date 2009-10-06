import FWCore.ParameterSet.Config as cms

ISpyPATMuon = cms.EDAnalyzer('ISpyPATMuon',
                             iSpyPATMuonTag = cms.InputTag("allLayer1Muons")
                             )
