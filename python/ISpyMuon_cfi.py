import FWCore.ParameterSet.Config as cms

ISpyMuon = cms.EDAnalyzer('ISpyMuon' ,
                          iSpyMuonTag = cms.InputTag("GLBMuons")
                          )
