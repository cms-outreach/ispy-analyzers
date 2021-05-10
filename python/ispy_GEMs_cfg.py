import os

# Stuff for singularity on lxplus
outPath = os.getenv('/afs/cern.ch/user/s/seungjun/public/ISPY/CMSSW_11_2_3/src')

if not outPath:
  outPath = ''
else:
  outPath += '/'

import FWCore.ParameterSet.Config as cms
from Configuration.StandardSequences.Eras import eras
process = cms.Process("ISPY", eras.Run3)
process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
##process.GlobalTag.globaltag = '112X_mcRun3_2021_design_v14'
##process.GlobalTag.globaltag = '112X_dataRun3_Prompt_v5' 
##process = cms.Process("ISPY")

process.load("Configuration.StandardSequences.MagneticField_cff")

#from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
from Configuration.AlCa.GlobalTag import GlobalTag                                ##
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase1_2021_realistic', '')## 

import FWCore.Utilities.FileUtils as FileUtils

process.source = cms.Source(
    'PoolSource',
    fileNames = cms.untracked.vstring(
    #'root://cmsxrootd.fnal.gov//store/data/Run2018D/DoubleMuon/AOD/PromptReco-v2/000/324/998/00000/AF519538-7FE5-4A4B-BD66-6FE4900CB5C6.root'
    # '/store/data/Commissioning2021/Cosmics/RAW-RECO/CosmicTP-PromptReco-v1/000/341/169/00000/095946f3-e67a-4b4f-a098-3432372b6d0e.root'
   #  '/store/group/alca_muonalign/run3MC/step2_501.root'
     '/store/data/Commissioning2021/Cosmics/RAW-RECO/CosmicTP-PromptReco-v1/000/341/343/00000/83bc114c-bd71-4ed7-beb1-fb6fac35e6e4.root'#for341343
    )

    )

from FWCore.MessageLogger.MessageLogger_cfi import *

process.add_(
        cms.Service("ISpyService",
                        outputFileName = cms.untracked.string('igOutput.ig'),
                        outputESFilename = cms.untracked.string('ES.ig'),
                        outputFilePath = cms.untracked.string(outPath),
                        outputIg = cms.untracked.bool(True),
                        outputMaxEvents = cms.untracked.int32(20),
                        )
        )

#process.options = cms.untracked.PSet(
#        SkipEvent = cms.untracked.vstring('ProductNotFound')
#            )

process.maxEvents = cms.untracked.PSet(
        input = cms.untracked.int32(20)
        )

process.load("ISpy.Analyzers.ISpyEvent_cfi")
process.load('ISpy.Analyzers.ISpyGEMDigi_cfi')
process.load('ISpy.Analyzers.ISpyGEMRecHit_cfi')
process.load('ISpy.Analyzers.ISpyGEMSegment_cfi')
process.load('ISpy.Analyzers.ISpyCSCRecHit2D_cfi')
process.load('ISpy.Analyzers.ISpyCSCSegment_cfi')
process.load('ISpy.Analyzers.ISpyMuon_cfi')
process.load('ISpy.Analyzers.ISpyVertex_cfi')

process.ISpyGEMRecHit.iSpyGEMRecHitTag = cms.InputTag("gemRecHits")
process.ISpyGEMSegment.iSpyGEMSegmentTag = cms.InputTag("gemSegments")
process.ISpyGEMDigi.iSpyGEMDigiTag = cms.InputTag("muonGEMDigis")
process.ISpyCSCRecHit2D.iSpyCSCRecHit2DTag = cms.InputTag("csc2DRecHits")
process.ISpyCSCSegment.iSpyCSCSegmentTag = cms.InputTag("cscSegments")
process.ISpyMuon.iSpyMuonTag = cms.InputTag("muons")
process.ISpyVertex.iSpyVertexTag = cms.InputTag('offlinePrimaryVertices')

process.iSpy = cms.Path(process.ISpyEvent*
                        process.ISpyGEMRecHit*
                        process.ISpyGEMDigi*
                        process.ISpyGEMSegment*
                        process.ISpyCSCRecHit2D*
                        process.ISpyCSCSegment*
                        process.ISpyMuon*
                        process.ISpyVertex)

process.schedule = cms.Schedule(process.iSpy)
