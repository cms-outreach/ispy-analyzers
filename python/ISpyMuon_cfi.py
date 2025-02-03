import FWCore.ParameterSet.Config as cms

ISpyMuon = cms.EDAnalyzer('ISpyMuon' ,
                          iSpyMuonTag = cms.InputTag("muons"),
                          ptMin = cms.double(2.0),
                          )
