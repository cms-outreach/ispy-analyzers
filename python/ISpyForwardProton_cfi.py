import FWCore.ParameterSet.Config as cms

ISpyForwardProton = cms.EDAnalyzer('ISpyForwardProton',
                                   iSpyForwardProtonTag = cms.InputTag("ctppsProtons:multiRP")
                            )
