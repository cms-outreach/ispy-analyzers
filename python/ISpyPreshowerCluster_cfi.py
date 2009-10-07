import FWCore.ParameterSet.Config as cms

ISpyPreshowerCluster = cms.EDAnalyzer('ISpyPreshowerCluster',
                                     iSpyPreshowerClusterTags = cms.VInputTag(
                                     cms.InputTag('multi5x5SuperClustersWithPreshower:preshowerXClusters'),
                                     cms.InputTag('multi5x5SuperClustersWithPreshower:preshowerYClusters')
                                     ))
