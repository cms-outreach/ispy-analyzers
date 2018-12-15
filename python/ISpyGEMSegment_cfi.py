import FWCore.ParameterSet.Config as cms

ISpyGEMSegment = cms.EDAnalyzer('ISpyGEMSegment' ,
                                iSpyGEMSegmentTag = cms.InputTag("gemSegments")
                                )
