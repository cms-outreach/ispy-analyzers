import FWCore.ParameterSet.Config as cms

process = cms.Process("IGUANA")

process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")
process.load("Configuration.StandardSequences.DigiToRaw_cff")
process.load("Configuration.StandardSequences.RawToDigi_cff")
process.load('Configuration/StandardSequences/VtxSmearedEarly10TeVCollision_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = "MC_31X_V1::All"

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring('/store/relval/CMSSW_3_3_2/RelValHiggs200ChargedTaus/GEN-SIM-DIGI-RAW-HLTDEBUG/STARTUP31X_V8-v2/0000/00240AC6-D1C7-DE11-818D-001A9281173C.root')
)

from FWCore.MessageLogger.MessageLogger_cfi import *

process.add_(
    cms.Service("ISpyService",
    outputFileName = cms.untracked.string('RelValHiggs200ChargedTaus.ig'),
    outputMaxEvents = cms.untracked.int32 (10),
    outputMaxTime = cms.untracked.int32 (1),
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
process.ISpyTrackingRecHit.iSpyTrackingRecHitTags = cms.VInputTag(cms.InputTag('generalTracks'))y

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

#process.p3= cms.Path(process.RawToDigi)
#process.p4= cms.Path(process.reconstruction)
process.schedule = cms.Schedule(process.iSpy)
