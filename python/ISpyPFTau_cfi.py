import FWCore.ParameterSet.Config as cms

ISpyPFTau = cms.EDAnalyzer('ISpyPFTau',
                            iSpyPFTauTag = cms.InputTag('fixedConeHighEffPFTauProducer')
                           )
