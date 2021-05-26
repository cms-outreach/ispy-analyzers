import os

# Stuff for singularity on lxplus
outPath = os.getenv('ANALYSIS_OUTDIR')

if not outPath:
  outPath = ''
else:
  outPath += '/'

import FWCore.ParameterSet.Config as cms

process = cms.Process("ISPY")

process.load("Configuration.StandardSequences.MagneticField_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load("Configuration.StandardSequences.GeometryDB_cff")

from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_data', '')


process.source = cms.Source(
    'PoolSource',
    fileNames = cms.untracked.vstring(
  '/store/data/Run2018C/Charmonium/AOD/17Sep2018-v1/00000/C3AA42CA-5E0B-5441-A5F0-DEF7831040C0.root'
  ),
    )

from FWCore.MessageLogger.MessageLogger_cfi import *
process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 100

process.add_(
        cms.Service("ISpyService",
                        outputFileName = cms.untracked.string('AOD.ig'),
                        outputESFilename = cms.untracked.string('ES.ig'),
                        outputFilePath = cms.untracked.string(outPath),
                        outputIg = cms.untracked.bool(True),
                        outputMaxEvents = cms.untracked.int32(25),
                        )
        )

process.options = cms.untracked.PSet(
        SkipEvent = cms.untracked.vstring('ProductNotFound')
            )

process.maxEvents = cms.untracked.PSet(
        input = cms.untracked.int32(25)
        )

process.load("ISpy.Analyzers.ISpyEvent_cfi")
process.load('ISpy.Analyzers.ISpyCSCRecHit2D_cfi')
process.load('ISpy.Analyzers.ISpyCSCSegment_cfi')
process.load('ISpy.Analyzers.ISpyDTRecHit_cfi')
process.load('ISpy.Analyzers.ISpyDTRecSegment4D_cfi')
process.load('ISpy.Analyzers.ISpyEBRecHit_cfi')
process.load('ISpy.Analyzers.ISpyEERecHit_cfi')
process.load('ISpy.Analyzers.ISpyESRecHit_cfi')
process.load('ISpy.Analyzers.ISpyHBRecHit_cfi')
process.load('ISpy.Analyzers.ISpyHERecHit_cfi')
process.load('ISpy.Analyzers.ISpyHFRecHit_cfi')
process.load('ISpy.Analyzers.ISpyHORecHit_cfi')
process.load('ISpy.Analyzers.ISpyMET_cfi')
process.load('ISpy.Analyzers.ISpyPFMET_cfi')
process.load('ISpy.Analyzers.ISpyMuon_cfi')
process.load('ISpy.Analyzers.ISpyJet_cfi')
process.load('ISpy.Analyzers.ISpyPFJet_cfi')
process.load('ISpy.Analyzers.ISpyPhoton_cfi')
process.load('ISpy.Analyzers.ISpyRPCRecHit_cfi')
process.load('ISpy.Analyzers.ISpySuperCluster_cfi')

process.load('ISpy.Analyzers.ISpyTrackExtrapolation_cfi')
process.load('ISpy.Analyzers.ISpyTriggerEvent_cfi')
process.load('ISpy.Analyzers.ISpyVertex_cfi')
process.load('ISpy.Analyzers.ISpyVertexCompositeCandidate_cfi')

process.ISpyCSCRecHit2D.iSpyCSCRecHit2DTag = cms.InputTag("csc2DRecHits")
process.ISpyCSCSegment.iSpyCSCSegmentTag = cms.InputTag("cscSegments")
process.ISpyDTRecHit.iSpyDTRecHitTag = cms.InputTag('dt1DRecHits')
process.ISpyDTRecSegment4D.iSpyDTRecSegment4DTag = cms.InputTag('dt4DSegments')

process.ISpyEBRecHit.iSpyEBRecHitTag = cms.InputTag('reducedEcalRecHitsEB')
process.ISpyEERecHit.iSpyEERecHitTag = cms.InputTag('reducedEcalRecHitsEE')
process.ISpyESRecHit.iSpyESRecHitTag = cms.InputTag('reducedEcalRecHitsES')

process.ISpyHBRecHit.iSpyHBRecHitTag = cms.InputTag("reducedHcalRecHits:hbhereco")
process.ISpyHERecHit.iSpyHERecHitTag = cms.InputTag("reducedHcalRecHits:hbhereco")
process.ISpyHFRecHit.iSpyHFRecHitTag = cms.InputTag("reducedHcalRecHits:hfreco")
process.ISpyHORecHit.iSpyHORecHitTag = cms.InputTag("reducedHcalRecHits:horeco")

process.ISpyMET.iSpyMETTag = cms.InputTag("htMetIC5")
process.ISpyMuon.iSpyMuonTag = cms.InputTag("muons")

process.ISpyPhoton.iSpyPhotonTag = cms.InputTag('photons')
process.ISpyRPCRecHit.iSpyRPCRecHitTag = cms.InputTag("rpcRecHits")
process.ISpyVertex.iSpyPriVertexTag = cms.InputTag('offlinePrimaryVerticesWithBS')
process.ISpyVertex.iSpySecVertexTag = cms.InputTag('inclusiveSecondaryVertices')
process.ISpyVertexCompositeCandidate.iSpyVertexCompositeCandidateTag  = cms.InputTag('generalV0Candidates:Lambda')

process.ISpyTrackExtrapolation.iSpyTrackExtrapolationTag = cms.InputTag("trackExtrapolator")

process.iSpy = cms.Path(process.ISpyEvent*
                        process.ISpyCSCRecHit2D*
                        process.ISpyCSCSegment*
                        process.ISpyDTRecHit*
                        process.ISpyDTRecSegment4D*
                        process.ISpyEBRecHit*
                        process.ISpyEERecHit*
                        process.ISpyESRecHit*
                        process.ISpyHBRecHit*
                        process.ISpyHERecHit*
                        process.ISpyHFRecHit*
                        process.ISpyHORecHit*
                        process.ISpyMuon*
                        process.ISpyPFJet*
                        process.ISpyPFMET*
                        process.ISpyPhoton*
                        process.ISpyRPCRecHit*
                        process.ISpyTrackExtrapolation*
                        process.ISpyVertexCompositeCandidate*
                        process.ISpyVertex)

process.schedule = cms.Schedule(process.iSpy)
