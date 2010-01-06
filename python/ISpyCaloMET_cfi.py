import FWCore.ParameterSet.Config as cms

ISpyCaloMET = cms.EDAnalyzer('ISpyCaloMET',
                            iSpyCaloMETTag = cms.InputTag('met')
                           )
