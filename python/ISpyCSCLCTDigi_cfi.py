import FWCore.ParameterSet.Config as cms

ISpyCSCLCTDigi = cms.EDAnalyzer('ISpyCSCLCTDigi',
                                 iSpyCSCLCTDigiTag = cms.InputTag('csctfDigis')
                                 )
