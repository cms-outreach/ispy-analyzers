import FWCore.ParameterSet.Config as cms

process = cms.Process("IGUANA")

process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'STARTUP_V5::All'

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('/store/relval/CMSSW_2_2_6/RelValHiggs200ChargedTaus/GEN-SIM-RECO/STARTUP_V9_v1/0001/EC69EE50-7F0B-DE11-9FED-000423D94534.root')
)

from FWCore.MessageLogger.MessageLogger_cfi import *

process.add_(
    cms.Service("ISpyService",
    outputFileName = cms.untracked.string('RelValHiggs200ChargedTaus.ig'),
    outputHost = cms.untracked.string('localhost'),
    outputPort = cms.untracked.uint32(9000),
    online = cms.untracked.bool(False),
    debug = cms.untracked.bool(True)
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10)
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
process.load("ISpy.Analyzers.ISpySimTrack_cfi")

process.ISpyTrack.iSpyTrackTag = cms.InputTag("generalTracks")
process.ISpyTrackingRecHit.iSpyTrackingRecHitTag = cms.InputTag("generalTracks")
process.ISpySiStripDigi.iSpySiStripDigiTag = cms.InputTag("siStripDigis","ZeroSuppressed")
process.ISpyMuon.iSpyMuonTag = cms.InputTag("muons")

process.p1 = cms.Path(process.ISpyEvent*
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
                      process.ISpySimTrack*
                      process.ISpyJet)

