import FWCore.ParameterSet.Config as cms

ISpyBeamSpot = cms.EDAnalyzer('ISpyBeamSpot',
                              iSpyBeamSpotTag = cms.InputTag('offlineBeamSpot')
                              )
