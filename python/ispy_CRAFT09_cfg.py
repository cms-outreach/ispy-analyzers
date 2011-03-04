import FWCore.ParameterSet.Config as cms

process = cms.Process("IGUANA")

process.load("Configuration.StandardSequences.Geometry_cff")
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('Configuration.StandardSequences.ReconstructionCosmics_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'GR09_31X_V6P::All'

process.source = cms.Source('PoolSource',
                            fileNames = cms.untracked.vstring('/store/data/Commissioning09/Cosmics/RECO/v5/000/106/019/FEB0439E-8072-DE11-A2BD-001D09F23A6B.root')
                            )


from FWCore.MessageLogger.MessageLogger_cfi import *

process.add_(
    cms.Service("ISpyService",
    outputFileName = cms.untracked.string('CRAFT09-CSCskim.ig'),
    outputMaxEvents = cms.untracked.int32(10),
    debug = cms.untracked.bool(True)
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1000)
)

process.load("ISpy.Analyzers.ISpyEventSetup_cfi")
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
process.load('ISpy.Analyzers.ISpyCSCRecHit2D_cfi')

process.ISpyDTRecHit.iSpyDTRecHitTag = cms.InputTag("dt1DRecHits")
process.ISpyRPCRecHit.iSpyRPCRecHitTag = cms.InputTag("rpcRecHits")
process.ISpyMET.iSpyMETTag = cms.InputTag('genMetIC5GenJets')
process.ISpyMuon.iSpyMuonTag = cms.InputTag('muons')
process.ISpySiStripDigi.iSpySiStripDigiTag = cms.InputTag('siStripDigis:ZeroSuppressed')
process.ISpyTrack.iSpyTrackTags = cms.VInputTag(cms.InputTag('cosmicMuons'))
process.ISpyTrackingRecHit.iSpyTrackingRecHitTags =  cms.VInputTag(cms.InputTag('cosmicMuons'))
process.ISpyCSCSegment.iSpyCSCSegmentTag = cms.InputTag("cscSegments")
process.ISpyCSCWireDigi.iSpyCSCWireDigiTag = cms.InputTag('muonCSCDigis:MuonCSCWireDigi')
process.ISpyCSCStripDigi.iSpyCSCStripDigiTag = cms.InputTag('muonCSCDigis:MuonCSCStripDigi')

process.load("DPGAnalysis/Skims/CSCSkim_cfi")

process.iSpy = cms.Path(process.cscSkim*
                       process.ISpyEvent*
                       process.ISpyEventSetup*
                       process.ISpyBasicCluster*
                       process.ISpyCSCSegment*
                       process.ISpyCSCStripDigi*
                       process.ISpyCSCWireDigi*
                       process.ISpyCSCRecHit2D*
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
                       process.ISpyMuon*
                       process.ISpyPixelDigi*
                       process.ISpySiPixelCluster*
                       process.ISpySiPixelRecHit*
                       process.ISpySiStripCluster*
                       process.ISpySiStripDigi*
                       process.ISpyL1GlobalTriggerReadoutRecord*
                       process.ISpyTriggerEvent)

process.schedule = cms.Schedule(process.iSpy)
