import FWCore.ParameterSet.Config as cms

ISpyPATMuon = cms.EDAnalyzer('ISpyPATMuon',
                             iSpyPATMuonTag = cms.InputTag("allLayer1Muons"),
                             isAOD = cms.untracked.bool(False),
                             ptMin = cms.double(2.0), 
                             )
