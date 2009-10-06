import FWCore.ParameterSet.Config as cms

ISpyCSCSegment = cms.EDAnalyzer('ISpyCSCSegment' ,
                                iSpyCSCSegmentTag = cms.InputTag("cscSegments")
                                )
