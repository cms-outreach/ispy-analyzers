import FWCore.ParameterSet.Config as cms

ISpySuperCluster = cms.EDAnalyzer('ISpySuperCluster',
                                  iSpySuperClusterTag = cms.InputTag('hybridSuperClusters')
                                  )
