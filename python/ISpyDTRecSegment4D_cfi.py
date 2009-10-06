import FWCore.ParameterSet.Config as cms

ISpyDTRecSegment4D = cms.EDAnalyzer('ISpyDTRecSegment4D' ,
                                    iSpyDTRecSegment4DTag = cms.InputTag('dt4DSegments')
                                    )
