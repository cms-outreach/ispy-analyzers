import FWCore.ParameterSet.Config as cms

ISpyCSCCorrelatedLCTDigi = cms.EDAnalyzer('ISpyCSCCorrelatedLCTDigi',
                                          iSpyCSCCorrelatedLCTDigiTag = cms.InputTag('muonCSCDigis:MuonCSCCorrelatedLCTDigi')
                                          )
