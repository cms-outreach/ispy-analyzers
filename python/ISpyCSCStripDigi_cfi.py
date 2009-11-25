import FWCore.ParameterSet.Config as cms

ISpyCSCStripDigi = cms.EDAnalyzer('ISpyCSCStripDigi',
                                  iSpyCSCStripDigiTag = cms.InputTag('muonCSCDigis:MuonCSCStripDigi'),
                                  thresholdOffset = cms.int32(9)
                                  )
