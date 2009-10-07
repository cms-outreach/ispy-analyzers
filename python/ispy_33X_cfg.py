import FWCore.ParameterSet.Config as cms

process = cms.Process("IGUANA")

process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load('Configuration.StandardSequences.VtxSmearedEarly10TeVCollision_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.GlobalTag.globaltag = 'MC_31X_V8::All'

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring('/store/relval/CMSSW_3_3_0_pre5/RelValTTbar/GEN-SIM-RECO/MC_31X_V8-v1/0004/EA71D4DD-6FAB-DE11-8A7F-001D09F23174.root')
)

from FWCore.MessageLogger.MessageLogger_cfi import *

process.add_(
    cms.Service("ISpyService",
    outputFileName = cms.untracked.string('RelValTTbar.ig'),
    online = cms.untracked.bool(False),
    debug = cms.untracked.bool(True)
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(5)
)

process.load("ISpy.Analyzers.ISpyCaloTower_cfi")
process.load("ISpy.Analyzers.ISpyCSCSegment_cfi")
process.load('ISpy.Analyzers.ISpyCSCStripDigi_cfi')
process.load('ISpy.Analyzers.ISpyCSCWireDigi_cfi')
process.load('ISpy.Analyzers.ISpyCSCRecHit2D_cfi')
process.load("ISpy.Analyzers.ISpyDTRecSegment4D_cfi")
process.load('ISpy.Analyzers.ISpyDTDigi_cfi')
process.load('ISpy.Analyzers.ISpyDTRecHit_cfi')
process.load('ISpy.Analyzers.ISpyRPCRecHit_cfi')
process.load('ISpy.Analyzers.ISpyEBRecHit_cfi')
process.load('ISpy.Analyzers.ISpyEERecHit_cfi')
process.load('ISpy.Analyzers.ISpyESRecHit_cfi')
process.load("ISpy.Analyzers.ISpyEvent_cfi")
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
process.load('ISpy.Analyzers.ISpyTrackingParticle_cfi')
process.load('ISpy.Analyzers.ISpyTriggerEvent_cfi')
process.load('ISpy.Analyzers.ISpyL1GlobalTriggerReadoutRecord_cfi')
process.load('ISpy.Analyzers.ISpyGsfTrack_cfi')
process.load('ISpy.Analyzers.ISpyPhoton_cfi')
process.load('ISpy.Analyzers.ISpyElectron_cfi')
process.load('ISpy.Analyzers.ISpyGsfElectron_cfi')
process.load('ISpy.Analyzers.ISpyPreshowerCluster_cfi')
process.load('ISpy.Analyzers.ISpyCaloCluster_cfi')
process.load('ISpy.Analyzers.ISpySuperCluster_cfi')

process.ISpyCSCSegment.iSpyCSCSegmentTag = cms.InputTag("cscSegments")
process.ISpyDTRecHit.iSpyDTRecHitTag = cms.InputTag("dt1DRecHits")
process.ISpyDTDigi.iSpyDTDigiTag = cms.InputTag('simMuonDTDigis')
process.ISpyDTRecHit.iSpyDTRecHitTag = cms.InputTag('dt1DRecHits')
process.ISpyMET.iSpyMETTag = cms.InputTag('genMetIC5GenJets')
process.ISpyMuon.iSpyMuonTag = cms.InputTag('muons')
process.ISpySiStripDigi.iSpySiStripDigiTag = cms.InputTag('siStripDigis:ZeroSuppressed')
process.ISpyTrackingRecHit.iSpyTrackingRecHitTag = cms.InputTag('generalTracks')

process.iSpy = cms.Path(process.ISpyEvent*
                        process.ISpyTrack*
                        process.ISpyTrackingRecHit*
                        process.ISpyCSCSegment*
                        process.ISpyCSCWireDigi*
                        process.ISpyCSCStripDigi*
                        process.ISpyCSCRecHit2D*
                        process.ISpyCaloTower*
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
                        process.ISpyMET*
                        process.ISpySiPixelCluster*
                        process.ISpySiPixelRecHit*
                        process.ISpySiStripCluster*
                        process.ISpySiStripDigi*
                        process.ISpyJet*
                        process.ISpyTrackingParticle*
                        process.ISpyTriggerEvent*
                        process.ISpyL1GlobalTriggerReadoutRecord*
                        process.ISpyMuon*
                        process.ISpyElectron*
                        process.ISpyGsfElectron*
                        process.ISpyPhoton*
                        process.ISpyGsfTrack*
                        process.ISpyPreshowerCluster*
                        process.ISpyCaloCluster*
                        process.ISpySuperCluster)

process.schedule = cms.Schedule(process.iSpy)
