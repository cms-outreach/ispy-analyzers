import FWCore.ParameterSet.Config as cms

ISpyMuon = cms.EDAnalyzer('ISpyMuon' ,
                          iSpyMuonTag = cms.InputTag("muons"),
                          isAOD = cms.untracked.bool(False)
                          )
