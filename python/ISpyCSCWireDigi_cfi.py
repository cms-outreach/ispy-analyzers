import FWCore.ParameterSet.Config as cms

ISpyCSCWireDigi = cms.EDAnalyzer('ISpyCSCWireDigi',
                                 iSpyCSCWireDigiTag = cms.InputTag('muonCSCDigis:MuonCSCWireDigi')
                                 )
