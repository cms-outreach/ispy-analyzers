import FWCore.ParameterSet.Config as cms

ISpyVertex = cms.EDAnalyzer('ISpyVertex',
                            iSpyVertexTag = cms.InputTag('offlinePrimaryVertices')
                           )
