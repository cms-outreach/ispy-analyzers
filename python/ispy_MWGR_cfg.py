import FWCore.ParameterSet.Config as cms

process = cms.Process("IGUANA")

process.load("Configuration.StandardSequences.Geometry_cff")
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load("Configuration.StandardSequences.ReconstructionCosmics_cff")
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('Configuration.StandardSequences.ReconstructionCosmics_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'GR09_31X_V2P::All'

process.source = cms.Source("NewEventStreamFileReader",
fileNames = cms.untracked.vstring('/store/streamer/MWGR_29/Express/000/106/019/MWGR_29.00106019.0001.Express.storageManager.01.0000.dat',
                                  '/store/streamer/MWGR_29/Express/000/106/019/MWGR_29.00106019.0001.Express.storageManager.02.0000.dat',
                                  '/store/streamer/MWGR_29/Express/000/106/019/MWGR_29.00106019.0001.Express.storageManager.03.0000.dat',
                                  '/store/streamer/MWGR_29/Express/000/106/019/MWGR_29.00106019.0001.Express.storageManager.04.0000.dat',
                                  '/store/streamer/MWGR_29/Express/000/106/019/MWGR_29.00106019.0001.Express.storageManager.05.0000.dat',
                                  '/store/streamer/MWGR_29/Express/000/106/019/MWGR_29.00106019.0001.Express.storageManager.06.0000.dat',
                                  '/store/streamer/MWGR_29/Express/000/106/019/MWGR_29.00106019.0001.Express.storageManager.07.0000.dat')
)

from FWCore.MessageLogger.MessageLogger_cfi import *

process.add_(
    cms.Service("ISpyService",
    outputFileName = cms.untracked.string('run106019-2.ig'),
    online = cms.untracked.bool(True),
    debug = cms.untracked.bool(True)
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(5)
)

process.load("ISpy.Analyzers.ISpyCaloTower_cfi")
process.load("ISpy.Analyzers.ISpyCSCSegment_cfi")
process.load("ISpy.Analyzers.ISpyDTRecSegment4D_cfi")
process.load('ISpy.Analyzers.ISpyDTDigi_cfi')
process.load('ISpy.Analyzers.ISpyDTRecHit_cfi')
process.load('ISpy.Analyzers.ISpyRPCRecHit_cfi')
process.load("ISpy.Analyzers.ISpyEcalRecHit_cfi")
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

process.ISpyCSCSegment.iSpyCSCSegmentTag = cms.InputTag("muonCSCSegments")
process.ISpyDTRecHit.iSpyDTRecHitTag = cms.InputTag("dt1DRecHits")
process.ISpyRPCRecHit.iSpyRPCRecHitTag = cms.InputTag("rpcRecHits")
process.ISpyMET.iSpyMETTag = cms.InputTag('genMetIC5GenJets')
process.ISpyMuon.iSpyMuonTag = cms.InputTag('muons')
process.ISpySiStripDigi.iSpySiStripDigiTag = cms.InputTag('siStripDigis:ZeroSuppressed')
process.ISpyTrack.iSpyTrackTags = cms.VInputTag(cms.InputTag('cosmicMuons'))
process.ISpyTrackingRecHit.iSpyTrackingRecHitTag = cms.InputTag('cosmicMuons')

process.iSpy = cms.Path(process.ISpyEvent*
                       process.ISpyTrack*
                       process.ISpyTrackingRecHit*
                       process.ISpyCSCSegment*
                       process.ISpyCaloTower*
                       process.ISpyDTRecSegment4D*
                       process.ISpyDTDigi*
                       process.ISpyDTRecHit*
                       process.ISpyRPCRecHit*
                       process.ISpyEcalRecHit*
                       process.ISpyHBRecHit*
                       process.ISpyHERecHit*
                       process.ISpyHFRecHit*
                       process.ISpyHORecHit*
                       process.ISpyMET*
                       process.ISpyMuon*
                       process.ISpySiPixelCluster*
                       process.ISpySiPixelRecHit*
                       process.ISpySiStripCluster*
                       process.ISpySiStripDigi*
                       process.ISpyJet)

process.p3= cms.Path(process.RawToDigi)
process.p4= cms.Path(process.reconstructionCosmics)
process.schedule = cms.Schedule(process.p3,process.p4,process.iSpy)
