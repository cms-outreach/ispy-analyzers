import FWCore.ParameterSet.Config as cms

ISpyCaloCluster = cms.EDAnalyzer('ISpyCaloCluster' ,
                                 iSpyCaloClusterTag = cms.InputTag("hybridSuperClusters:hybridBarrelBasicClusters")
                                )
                                 
