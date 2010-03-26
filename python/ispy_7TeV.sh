#!/bin/bash

if [ $# -ne 1 ]
then
    echo "Usage: dumpDataFile.sh  [*.root]"
    exit
fi

FILENAME=$1

echo "Processing "${MAXNEVENTS}

CFGFILE=${PWD}/ispy_7TeV.py
cat > ${CFGFILE}<<EOF


import FWCore.ParameterSet.Config as cms

process = cms.Process("ISPY")
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.GlobalTag.globaltag = 'STARTUP3X_V14::All'

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring('file:${FILENAME}'))
  
from FWCore.MessageLogger.MessageLogger_cfi import *

process.add_(
    cms.Service("ISpyService",
    outputFileName = cms.untracked.string('ispy.ig'),
    outputIg = cms.untracked.bool(True),
    outputMaxEvents = cms.untracked.int32(10), # These are the number of events per ig file 
    online = cms.untracked.bool(False),
    debug = cms.untracked.bool(False)
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10) # These are the number of events to cycle through in the input root file
)

# Load everything here instead of using ISpy_Producer so that we can
# explicitly see what is running and change parameters easily in one
# place if needed

process.load("ISpy.Analyzers.ISpyEvent_cfi")
process.load('ISpy.Analyzers.ISpyBasicCluster_cfi')
process.load('ISpy.Analyzers.ISpyCSCRecHit2D_cfi')
process.load('ISpy.Analyzers.ISpyCSCSegment_cfi')
process.load('ISpy.Analyzers.ISpyCSCStripDigi_cfi')
process.load('ISpy.Analyzers.ISpyCSCWireDigi_cfi')
process.load('ISpy.Analyzers.ISpyCaloCluster_cfi')
process.load('ISpy.Analyzers.ISpyCaloTower_cfi')
process.load('ISpy.Analyzers.ISpyCaloMET_cfi')
process.load('ISpy.Analyzers.ISpyCaloTau_cfi')
process.load('ISpy.Analyzers.ISpyDTDigi_cfi')
process.load('ISpy.Analyzers.ISpyDTRecHit_cfi')
process.load('ISpy.Analyzers.ISpyDTRecSegment4D_cfi')
process.load('ISpy.Analyzers.ISpyEBRecHit_cfi')
process.load('ISpy.Analyzers.ISpyEERecHit_cfi')
process.load('ISpy.Analyzers.ISpyESRecHit_cfi')
#process.load('ISpy.Analyzers.ISpyElectron_cfi')
process.load('ISpy.Analyzers.ISpyGsfElectron_cfi')
process.load('ISpy.Analyzers.ISpyGsfPFRecTrack_cfi')
process.load('ISpy.Analyzers.ISpyGsfTrack_cfi')
process.load('ISpy.Analyzers.ISpyHBRecHit_cfi')
process.load('ISpy.Analyzers.ISpyHERecHit_cfi')
process.load('ISpy.Analyzers.ISpyHFRecHit_cfi')
process.load('ISpy.Analyzers.ISpyHORecHit_cfi')
process.load('ISpy.Analyzers.ISpyJet_cfi')
process.load('ISpy.Analyzers.ISpyL1GlobalTriggerReadoutRecord_cfi')
process.load('ISpy.Analyzers.ISpyMET_cfi')
process.load('ISpy.Analyzers.ISpyMuon_cfi')
#process.load('ISpy.Analyzers.ISpyPATElectron_cfi')
#process.load('ISpy.Analyzers.ISpyPATMuon_cfi')
#process.load('ISpy.Analyzers.ISpyPATPhoton_cfi')
process.load('ISpy.Analyzers.ISpyPFCluster_cfi')
process.load('ISpy.Analyzers.ISpyPFRecHit_cfi')
process.load('ISpy.Analyzers.ISpyPFRecTrack_cfi')
process.load('ISpy.Analyzers.ISpyPFJet_cfi')
process.load('ISpy.Analyzers.ISpyPFMET_cfi')
process.load('ISpy.Analyzers.ISpyPFTau_cfi')
process.load('ISpy.Analyzers.ISpyPhoton_cfi')
process.load('ISpy.Analyzers.ISpyPixelDigi_cfi')
process.load('ISpy.Analyzers.ISpyPreshowerCluster_cfi')
process.load('ISpy.Analyzers.ISpyRPCRecHit_cfi')
process.load('ISpy.Analyzers.ISpySiPixelCluster_cfi')
process.load('ISpy.Analyzers.ISpySiPixelRecHit_cfi')
process.load('ISpy.Analyzers.ISpySiStripCluster_cfi')
process.load('ISpy.Analyzers.ISpySiStripDigi_cfi')
process.load('ISpy.Analyzers.ISpySuperCluster_cfi')
process.load('ISpy.Analyzers.ISpyTrack_cfi')
process.load('ISpy.Analyzers.ISpyTrackingRecHit_cfi')
process.load('ISpy.Analyzers.ISpyTriggerEvent_cfi')
process.load('ISpy.Analyzers.ISpyVertex_cfi')

process.ISpyBasicCluster.iSpyBasicClusterTag = cms.InputTag("hybridSuperClusters:hybridBarrelBasicClusters")
process.ISpyCSCRecHit2D.iSpyCSCRecHit2DTag = cms.InputTag("csc2DRecHits")
process.ISpyCSCSegment.iSpyCSCSegmentTag = cms.InputTag("cscSegments")
process.ISpyCSCStripDigi.iSpyCSCStripDigiTag = cms.InputTag('muonCSCDigis:MuonCSCStripDigi')
process.ISpyCSCStripDigi.thresholdOffset = cms.int32(9)
process.ISpyCSCWireDigi.iSpyCSCWireDigiTag = cms.InputTag('muonCSCDigis:MuonCSCWireDigi')
process.ISpyCaloCluster.iSpyCaloClusterTag = cms.InputTag("hybridSuperClusters:hybridBarrelBasicClusters")
process.ISpyCaloTower.iSpyCaloTowerTag = cms.InputTag('towerMaker')
process.ISpyCaloMET.iSpyCaloMETTag = cms.InputTag('met')
process.ISpyCaloTau.iSpyCaloTauTag = cms.InputTag('caloRecoTauProducer')
process.ISpyDTDigi.iSpyDTDigiTag = cms.InputTag('muonDTDigis')
process.ISpyDTRecHit.iSpyDTRecHitTag = cms.InputTag('dt1DRecHits')
process.ISpyDTRecSegment4D.iSpyDTRecSegment4DTag = cms.InputTag('dt4DSegments')
process.ISpyEBRecHit.iSpyEBRecHitTag = cms.InputTag('ecalRecHit:EcalRecHitsEB')
process.ISpyEERecHit.iSpyEERecHitTag = cms.InputTag('ecalRecHit:EcalRecHitsEE')
process.ISpyESRecHit.iSpyESRecHitTag = cms.InputTag('ecalPreshowerRecHit:EcalRecHitsES')
#process.ISpyElectron.iSpyElectronTag = cms.InputTag('hltPixelMatchElectronsL1Iso')
process.ISpyGsfElectron.iSpyGsfElectronTag = cms.InputTag('gsfElectrons')
process.ISpyGsfPFRecTrack.iSpyGsfPFRecTrackTag = cms.InputTag('pfTrackElec')
process.ISpyGsfTrack.iSpyGsfTrackTag = cms.InputTag('electronGsfTracks')
process.ISpyHBRecHit.iSpyHBRecHitTag = cms.InputTag("hbhereco")
process.ISpyHERecHit.iSpyHERecHitTag = cms.InputTag("hbhereco")
process.ISpyHFRecHit.iSpyHFRecHitTag = cms.InputTag("hfreco")
process.ISpyHORecHit.iSpyHORecHitTag = cms.InputTag("horeco")
process.ISpyJet.iSpyJetTag = cms.InputTag("iterativeCone5CaloJets")
process.ISpyL1GlobalTriggerReadoutRecord.iSpyL1GlobalTriggerReadoutRecordTag = cms.InputTag("gtDigis")
process.ISpyMET.iSpyMETTag = cms.InputTag("htMetIC5")
process.ISpyMuon.iSpyMuonTag = cms.InputTag("muons")
#process.ISpyPATElectron.iSpyPATElectronTag = cms.InputTag('allLayer1Electrons')
#process.ISpyPATMuon.iSpyPATMuonTag = cms.InputTag('allLayer1Muons')
#process.ISpyPATPhoton.iSpyPATPhotonTag = cms.InputTag('allLayer1Photons')
process.ISpyPFCluster.iSpyPFClusterTag = cms.InputTag("particleFlowClusterECAL")
process.ISpyPFRecHit.iSpyPFRecHitTag = cms.InputTag("particleFlowRecHitECAL")
process.ISpyPFRecTrack.iSpyPFRecTrackTag = cms.InputTag("trackerDrivenElectronSeeds")
process.ISpyPFJet.iSpyPFJetTag = cms.InputTag('iterativeCone5PFJets')
process.ISpyPFMET.iSpyPFMETTag = cms.InputTag('pfMet')
process.ISpyPFTau.iSpyPFTauTag = cms.InputTag('fixedConeHighEffPFTauProducer')
process.ISpyPhoton.iSpyPhotonTag = cms.InputTag('photons')
process.ISpyPixelDigi.iSpyPixelDigiTag = cms.InputTag("siPixelDigis")
process.ISpyPreshowerCluster.iSpyPreshowerClusterTags = cms.VInputTag(cms.InputTag('multi5x5SuperClustersWithPreshower:preshowerXClusters'), cms.InputTag('multi5x5SuperClustersWithPreshower:preshowerYClusters'))
process.ISpyRPCRecHit.iSpyRPCRecHitTag = cms.InputTag("rpcRecHits")
process.ISpySiPixelCluster.iSpySiPixelClusterTag = cms.InputTag("siPixelClusters")
process.ISpySiPixelRecHit.iSpySiPixelRecHitTag = cms.InputTag("siPixelRecHits")
process.ISpySiStripCluster.iSpySiStripClusterTag = cms.InputTag("siStripClusters")
process.ISpySiStripDigi.iSpySiStripDigiTag = cms.InputTag("siStripDigis:ZeroSuppressed")
process.ISpySuperCluster.iSpySuperClusterTag = cms.InputTag('hybridSuperClusters')
process.ISpyTrack.iSpyTrackTags = cms.VInputTag(cms.InputTag("generalTracks"))
process.ISpyTrackingRecHit.iSpyTrackingRecHitTags = cms.VInputTag(cms.InputTag("generalTracks"))
process.ISpyTriggerEvent.triggerEventTag = cms.InputTag('hltTriggerSummaryAOD')
process.ISpyTriggerEvent.triggerResultsTag = cms.InputTag('TriggerResults')
process.ISpyTriggerEvent.processName = cms.string('HLT')
process.ISpyVertex.iSpyVertexTag = cms.InputTag('offlinePrimaryVertices')

process.iSpy = cms.Path(process.ISpyEvent*
                        process.ISpyBasicCluster*
                        process.ISpyCSCRecHit2D*
                        process.ISpyCSCSegment*
                        process.ISpyCSCStripDigi*
                        process.ISpyCSCWireDigi*
                        process.ISpyCaloCluster*
                        process.ISpyCaloTower*
                        process.ISpyCaloMET*
                        process.ISpyCaloTau*
                        process.ISpyDTDigi*
                        process.ISpyDTRecHit*
                        process.ISpyDTRecSegment4D*
                        process.ISpyEBRecHit*
                        process.ISpyEERecHit*
                        process.ISpyESRecHit*
                        #process.ISpyElectron*
                        process.ISpyGsfElectron*
                        process.ISpyGsfPFRecTrack*
                        process.ISpyGsfTrack*
                        process.ISpyHBRecHit*
                        process.ISpyHERecHit*
                        process.ISpyHFRecHit*
                        process.ISpyHORecHit*
                        process.ISpyJet*
                        process.ISpyL1GlobalTriggerReadoutRecord*
                        process.ISpyMET*
                        process.ISpyMuon*
                        #process.ISpyPATElectron*
                        #process.ISpyPATMuon*
                        #process.ISpyPATPhoton*
                        process.ISpyPFCluster*
                        process.ISpyPFRecHit*
                        process.ISpyPFRecTrack*
                        process.ISpyPFJet*
                        process.ISpyPFMET*
                        process.ISpyPFTau*
                        process.ISpyPhoton*
                        process.ISpyPixelDigi*
                        process.ISpyPreshowerCluster*
                        process.ISpyRPCRecHit*
                        process.ISpySiPixelCluster*
                        process.ISpySiPixelRecHit*
                        process.ISpySiStripCluster*
                        process.ISpySiStripDigi*
                        process.ISpySuperCluster*
                        process.ISpyTrack*
                        process.ISpyTrackingRecHit*
                        process.ISpyTriggerEvent*
                        process.ISpyVertex)

process.schedule = cms.Schedule(process.iSpy)

EOF

cmsRun ${CFGFILE}
