import FWCore.ParameterSet.Config as cms

process = cms.Process("ISPY")

process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.GlobalTag.globaltag = '100X_dataRun2_Prompt_v5' 

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring('file:/eos/cms/store/group/visualization/run313133/run313133_ls0031_streamEvDOutput_dqmcluster.root')
                            ,eventsToProcess = cms.untracked.VEventRange('313133:3215')
)
 
from FWCore.MessageLogger.MessageLogger_cfi import *

process.add_(
    cms.Service("ISpyService",
    outputFileName = cms.untracked.string('splash2018.ig'),
    outputMaxEvents = cms.untracked.int32(50)
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(50)
)

process.load('ISpy.Analyzers.ISpyEvent_cfi')
process.load('ISpy.Analyzers.ISpyCSCRecHit2D_cfi')
process.load('ISpy.Analyzers.ISpyCSCSegment_cfi')
process.load('ISpy.Analyzers.ISpyEBRecHit_cfi')
process.load('ISpy.Analyzers.ISpyEERecHit_cfi')

process.load('ISpy.Analyzers.ISpyHBRecHit_cfi')
process.load('ISpy.Analyzers.ISpyHERecHit_cfi')
process.load('ISpy.Analyzers.ISpyHFRecHit_cfi')
process.load('ISpy.Analyzers.ISpyHORecHit_cfi')

process.ISpyCSCRecHit2D.iSpyCSCRecHit2DTag = cms.InputTag("csc2DRecHits")
process.ISpyCSCSegment.iSpyCSCSegmentTag = cms.InputTag("cscSegments")
process.ISpyEBRecHit.iSpyEBRecHitTag = cms.InputTag('ecalRecHit:EcalRecHitsEB')
process.ISpyEERecHit.iSpyEERecHitTag = cms.InputTag('ecalRecHit:EcalRecHitsEE')
process.ISpyHBRecHit.iSpyHBRecHitTag = cms.InputTag("hbhereco")
process.ISpyHERecHit.iSpyHERecHitTag = cms.InputTag("hbhereco")
process.ISpyHFRecHit.iSpyHFRecHitTag = cms.InputTag("hfreco")
process.ISpyHORecHit.iSpyHORecHitTag = cms.InputTag("horeco")

process.iSpy = cms.Path(process.ISpyEvent*
                        process.ISpyCSCRecHit2D*
                        process.ISpyCSCSegment*
                        process.ISpyEBRecHit*
                        process.ISpyEERecHit*
                        process.ISpyHBRecHit*
                        process.ISpyHERecHit*
                        process.ISpyHFRecHit*
                        process.ISpyHORecHit)
                      
process.schedule = cms.Schedule(process.iSpy)

