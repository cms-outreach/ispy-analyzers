import FWCore.ParameterSet.Config as cms

process = cms.Process("IGUANA")

process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.load("Configuration.StandardSequences.RawToDigi_Data_cff")
process.load("Configuration.StandardSequences.ReconstructionCosmics_cff")
process.GlobalTag.globaltag = 'CRAFT_V2H::All'
process.GlobalTag.connect = "frontier://(proxyurl=http://localhost:3128)(serverurl=http://frontier1.cms:8000/FrontierOnProd)(serverurl=http://frontier2.cms:8000/FrontierOnProd)(retrieve-ziplevel=0)/CMS_COND_21X_GLOBALTAG"


process.load("DQM.Integration.test.inputsource_playback_cfi")
process.EventStreamHttpReader.consumerName = 'Event Display'
#process.maxEvents = cms.untracked.PSet(
#    input = cms.untracked.int32(-1)
#)
#process.source = cms.Source("EventStreamHttpReader",
#	sourceURL = cms.string("http://localhost:5000/urn:xdaq-application:lid=29"),
#	sourceURL = cms.string('http://srv-c2d05-14.cms:22100/urn:xdaq-application:lid=30'),
#	sourceURL = cms.string('http://srv-c2d05-05.cms:50082/urn:xdaq-application:lid=29'),
#	consumerPriority = cms.untracked.string('normal'),
#	max_event_size = cms.int32(7000000),
#	consumerName = cms.untracked.string('Event Display'),
#	max_queue_depth = cms.int32(5),
#   SelectHLTOutput = cms.untracked.string("hltOutputDQM"),
#	maxEventRequestRate = cms.untracked.double(12.0),
#	SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('*')),
#	headerRetryInterval = cms.untracked.int32(3)
#)

from FWCore.MessageLogger.MessageLogger_cfi import *

process.add_(
    cms.Service("ISpyService",
    outputFileName = cms.untracked.string('/home/dqmdevlocal/output/Higgs200ChargedTaus.ig'),
    outputESFileName = cms.untracked.string('/home/dqmdevlocal/output/ispy_FrontierConditions_GlobalTag_STARTUP_V5.ig'),
    outputMaxEvents = cms.untracked.int32(3000),
    outputHost = cms.untracked.string('localhost'),
    outputPort = cms.untracked.uint32(9000),
    online = cms.untracked.bool(True),
    debug = cms.untracked.bool(False)
    )
)


process.load("ISpy.Analyzers.ISpyCaloTower_cfi")
process.load("ISpy.Analyzers.ISpyCSCSegment_cfi")
process.load("ISpy.Analyzers.ISpyDTDigi_cfi")
process.load("ISpy.Analyzers.ISpyDTRecHit_cfi")
process.load("ISpy.Analyzers.ISpyDTRecSegment4D_cfi")
process.load("ISpy.Analyzers.ISpyRPCRecHit_cfi")
process.load("ISpy.Analyzers.ISpyEcalRecHit_cfi")
process.load("ISpy.Analyzers.ISpyEvent_cfi")
process.load("ISpy.Analyzers.ISpyEventSetup_cfi")
process.load("ISpy.Analyzers.ISpyHBRecHit_cfi")
process.load("ISpy.Analyzers.ISpyHERecHit_cfi")
process.load("ISpy.Analyzers.ISpyHFRecHit_cfi")
process.load("ISpy.Analyzers.ISpyHORecHit_cfi")
process.load("ISpy.Analyzers.ISpyJet_cfi")
process.load("ISpy.Analyzers.ISpyMET_cfi")
process.load("ISpy.Analyzers.ISpyMuon_cfi")
process.load("ISpy.Analyzers.ISpyPixelDigi_cfi")
process.load("ISpy.Analyzers.ISpySiPixelCluster_cfi")
process.load("ISpy.Analyzers.ISpySiPixelRecHit_cfi")
process.load("ISpy.Analyzers.ISpySiStripCluster_cfi")
process.load("ISpy.Analyzers.ISpySiStripDigi_cfi")
process.load("ISpy.Analyzers.ISpyTrack_cfi")
process.load("ISpy.Analyzers.ISpyTrackingRecHit_cfi")

process.ISpyTrack.iSpyTrackTag = cms.InputTag("globalCosmicMuons")
process.ISpyTrackingRecHit.iSpyTrackingRecHitTag = cms.InputTag("globalCosmicMuons")
process.ISpySiStripDigi.iSpySiStripDigiTag = cms.InputTag("siStripDigis","ZeroSuppressed")
process.ISpyMuon.iSpyMuonTag = cms.InputTag("muons")


process.p1 = cms.Path(process.RawToDigi_woGCT*
                      process.reconstructionCosmics*
                      process.ISpyEvent*
                      process.ISpyEventSetup*
                      process.ISpyTrack*
                      process.ISpyTrackingRecHit*
                      process.ISpyCSCSegment*
                      process.ISpyCaloTower*
                      process.ISpyDTRecSegment4D*
                      process.ISpyDTRecHit*
                      process.ISpyDTDigi*
                      process.ISpyRPCRecHit*
                      process.ISpyEcalRecHit*
                      process.ISpyHBRecHit*
                      process.ISpyHERecHit*
                      process.ISpyHFRecHit*
                      process.ISpyHORecHit*
                      process.ISpyMET*
                      process.ISpyMuon*
                      process.ISpyPixelDigi*
                      process.ISpySiPixelCluster*
                      process.ISpySiPixelRecHit*
                      process.ISpySiStripCluster*
                      process.ISpySiStripDigi*
                      process.ISpyJet)

