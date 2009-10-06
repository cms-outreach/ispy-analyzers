import FWCore.ParameterSet.Config as cms

ISpySiPixelCluster = cms.EDAnalyzer('ISpySiPixelCluster' ,
                                    iSpySiPixelClusterTag = cms.InputTag("siPixelClusters")
                                    )
