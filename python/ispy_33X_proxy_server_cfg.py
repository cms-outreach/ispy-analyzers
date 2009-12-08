import FWCore.ParameterSet.Config as cms
import socket

process = cms.Process("IGUANA")

process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff")
process.load("Configuration.StandardSequences.ReconstructionCosmics_cff")
process.load("Configuration.StandardSequences.RawToDigi_Data_cff")
process.load('Configuration/StandardSequences/VtxSmearedEarly10TeVCollision_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = "GR09_P_V4::All"

process.load('L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMenuConfig_cff')
process.es_prefer_l1GtParameters = cms.ESPrefer('L1GtTriggerMenuXmlProducer','l1GtTriggerMenuXml')
process.load('L1TriggerConfig.L1GtConfigProducers.Luminosity.lumi1031.L1Menu_MC2009_v2_L1T_Scales_20080922_Imp0_Unprescaled_cff')

process.source = cms.Source("EventStreamHttpReader",
   sourceURL = cms.string('http://localhost:50082/urn:xdaq-application:lid=29'),
   consumerPriority = cms.untracked.string('normal'),
   max_event_size = cms.int32(7000000),
   consumerName = cms.untracked.string('Playback iSpy Source'),
   SelectHLTOutput = cms.untracked.string('hltOutputDQM'),
   max_queue_depth = cms.int32(5),
   maxEventRequestRate = cms.untracked.double(10.0),
   SelectEvents = cms.untracked.PSet(
       SelectEvents = cms.vstring('*')
   ),
   headerRetryInterval = cms.untracked.int32(3)
)

from FWCore.MessageLogger.MessageLogger_cfi import *

process.add_(
    cms.Service("ISpyService",
    outputFileName = cms.untracked.string('ProxyServer.ig'),
    outputMaxEvents = cms.untracked.int32 (10),
    online = cms.untracked.bool(True),
    debug = cms.untracked.bool(True)
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.load("ISpy.Analyzers.ISpyEvent_cfi")
process.load("ISpy.Analyzers.ISpyBasicCluster_cfi")
process.load("ISpy.Analyzers.ISpyCSCSegment_cfi")
process.load("ISpy.Analyzers.ISpyCSCStripDigi_cfi")
process.load("ISpy.Analyzers.ISpyCSCWireDigi_cfi")
process.load("ISpy.Analyzers.ISpyCaloTower_cfi")
process.load('ISpy.Analyzers.ISpyDTDigi_cfi')
process.load('ISpy.Analyzers.ISpyDTRecHit_cfi')
process.load("ISpy.Analyzers.ISpyDTRecSegment4D_cfi")
process.load("ISpy.Analyzers.ISpyEBRecHit_cfi")
process.load("ISpy.Analyzers.ISpyEERecHit_cfi")
process.load("ISpy.Analyzers.ISpyESRecHit_cfi")
process.load("ISpy.Analyzers.ISpyHBRecHit_cfi")
process.load("ISpy.Analyzers.ISpyHERecHit_cfi")
process.load("ISpy.Analyzers.ISpyHFRecHit_cfi")
process.load("ISpy.Analyzers.ISpyHORecHit_cfi")
process.load("ISpy.Analyzers.ISpyJet_cfi")
process.load("ISpy.Analyzers.ISpyL1GlobalTriggerReadoutRecord_cfi")
process.load("ISpy.Analyzers.ISpyMET_cfi")
process.load("ISpy.Analyzers.ISpyMuon_cfi")
process.load("ISpy.Analyzers.ISpyPixelDigi_cfi")
process.load('ISpy.Analyzers.ISpyRPCRecHit_cfi')
process.load("ISpy.Analyzers.ISpySiPixelCluster_cfi")
process.load("ISpy.Analyzers.ISpySiPixelRecHit_cfi")
process.load("ISpy.Analyzers.ISpySiStripCluster_cfi")
process.load("ISpy.Analyzers.ISpySiStripDigi_cfi")
process.load("ISpy.Analyzers.ISpyTrack_cfi")
process.load("ISpy.Analyzers.ISpyTrackingRecHit_cfi")
process.load("ISpy.Analyzers.ISpyTriggerEvent_cfi")

process.ISpyCSCStripDigi.iSpyCSCStripDigiTag = cms.InputTag("muonCSCDigis:MuonCSCStripDigi")
process.ISpyCSCWireDigi.iSpyCSCWireDigiTag = cms.InputTag("muonCSCDigis:MuonCSCWireDigi")
process.ISpyCSCSegment.iSpyCSCSegmentTag = cms.InputTag("muonCSCSegments")
process.ISpyDTRecHit.iSpyDTRecHitTag = cms.InputTag("dt1DRecHits")
process.ISpyRPCRecHit.iSpyRPCRecHitTag = cms.InputTag("rpcRecHits")
process.ISpyMET.iSpyMETTag = cms.InputTag('genMetIC5GenJets')
process.ISpyMuon.iSpyMuonTag = cms.InputTag('muons')
process.ISpySiStripDigi.iSpySiStripDigiTag = cms.InputTag('siStripDigis:ZeroSuppressed')
process.ISpyTrack.iSpyTrackTags = cms.VInputTag(cms.InputTag('generalTracks'))
process.ISpyTrackingRecHit.iSpyTrackingRecHitTags = cms.VInputTag(cms.InputTag('generalTracks'))

process.iSpy = cms.Path(process.ISpyEvent*
                       process.ISpyBasicCluster*
                       process.ISpyCSCSegment*
                       process.ISpyCSCStripDigi*
                       process.ISpyCSCWireDigi*
                       process.ISpyCaloTower*
                       process.ISpyTrack*
                       process.ISpyTrackingRecHit*
                       process.ISpyDTRecSegment4D*
                       process.ISpyDTDigi*
                       process.ISpyDTRecHit*
                       process.ISpyRPCRecHit*
                       process.ISpyEBRecHit*
                       process.ISpyEERecHit*
                       process.ISpyESRecHit*
                       process.ISpyHBRecHit*
                       process.ISpyHERecHit*
                       process.ISpyHFRecHit*
                       process.ISpyHORecHit*
                       process.ISpyJet*
                       process.ISpyMET*
                       process.ISpyMuon*
                       process.ISpyPixelDigi*
                       process.ISpySiPixelCluster*
                       process.ISpySiPixelRecHit*
                       process.ISpySiStripCluster*
                       process.ISpySiStripDigi*
                       process.ISpyL1GlobalTriggerReadoutRecord*
                       process.ISpyTriggerEvent)

process.p3= cms.Path(process.RawToDigi)
process.p4= cms.Path(process.reconstructionCosmics)
process.schedule = cms.Schedule(process.p3,process.p4,process.iSpy)
