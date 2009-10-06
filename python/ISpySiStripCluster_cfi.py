import FWCore.ParameterSet.Config as cms

ISpySiStripCluster = cms.EDAnalyzer('ISpySiStripCluster' ,
                                    iSpySiStripClusterTag = cms.InputTag("siStripClusters")
                                    )
