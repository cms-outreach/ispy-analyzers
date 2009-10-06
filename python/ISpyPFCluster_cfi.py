import FWCore.ParameterSet.Config as cms

ISpyPFCluster = cms.EDAnalyzer('ISpyPFCluster' ,
                               iSpyPFClusterTag = cms.InputTag("particleFlowClusterECAL")
                               )
