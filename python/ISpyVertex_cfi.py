import FWCore.ParameterSet.Config as cms

ISpyVertex = cms.EDAnalyzer('ISpyVertex',
                            iSpyPriVertexTag = cms.InputTag('offlinePrimaryVertices'),
                            iSpySecVertexTag = cms.InputTag('inclusiveSecondaryVertices')
                           )
