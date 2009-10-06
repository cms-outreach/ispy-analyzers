import FWCore.ParameterSet.Config as cms

ISpyCSCStripDigi = cms.EDAnalyzer('ISpyCSCStripDigi',
                                  iSpyCSCStripDigiTag = cms.InputTag('MuonCSCStripDigi'),
                                  thresholdOffset = cms.int32(9)
                                  )
