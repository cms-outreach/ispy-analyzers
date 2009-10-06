import FWCore.ParameterSet.Config as cms

ISpyBasicCluster = cms.EDAnalyzer('ISpyBasicCluster' ,
                                  iSpyBasicClusterTag = cms.InputTag("hybridSuperClusters:hybridBarrelBasicClusters")
                                  )
                                 
