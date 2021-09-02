import FWCore.ParameterSet.Config as cms
import FWCore.PythonUtilities.LumiList as LumiList
import FWCore.Utilities.FileUtils as FileUtils

process = cms.Process("ISPY")
goodJSON = 'Cert_181530-183126_HI7TeV_PromptReco_Collisions11_JSON_MuonPhys.txt'
myLumis = LumiList.LumiList(filename = goodJSON).getCMSSWString().split(',')

process.load('Configuration.StandardSequences.Geometry_cff')
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.GlobalTag.globaltag = 'GR_R_44_V15::All'

files = FileUtils.loadListFromFile('CMS_HIRun2011_HIDiMuon_RECO_04Mar2013-v1_root_file_index.txt')

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
	#*files
	'root://eospublic.cern.ch//eos/opendata/cms/hidata/HIRun2011/HIDiMuon/RECO/04Mar2013-v1/10000/001C9519-BA87-E211-9483-842B2B6F81CC.root'
	)	
)

process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange()
process.source.lumisToProcess.extend(myLumis)

from FWCore.MessageLogger.MessageLogger_cfi import *

process.options = cms.untracked.PSet(
    SkipEvent = cms.untracked.vstring('ProductNotFound')
)

process.add_(
    cms.Service("ISpyService",
                outputFileName = cms.untracked.string('HIDiMuon.ig'),
                outputMaxEvents = cms.untracked.int32(10)
               )
)

# This number is the total number of events run over. "-1" indicates all.
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10)
)

process.load('ISpy.Analyzers.ISpyEvent_cfi')

process.load('ISpy.Analyzers.ISpyEBRecHit_cfi')
process.load('ISpy.Analyzers.ISpyEERecHit_cfi')
process.load('ISpy.Analyzers.ISpyESRecHit_cfi')

process.load('ISpy.Analyzers.ISpyHBRecHit_cfi')
process.load('ISpy.Analyzers.ISpyHERecHit_cfi')
process.load('ISpy.Analyzers.ISpyHFRecHit_cfi')
process.load('ISpy.Analyzers.ISpyHORecHit_cfi')

process.load('ISpy.Analyzers.ISpyTrack_cfi')
process.load('ISpy.Analyzers.ISpyTrackExtrapolation_cfi')
process.load('ISpy.Analyzers.ISpyMuon_cfi')


process.ISpyEBRecHit.iSpyEBRecHitTag = cms.InputTag('ecalRecHit:EcalRecHitsEB')
process.ISpyEERecHit.iSpyEERecHitTag = cms.InputTag('ecalRecHit:EcalRecHitsEE')
process.ISpyESRecHit.iSpyESRecHitTag = cms.InputTag('ecalRecHit:EcalRecHitsES')

process.ISpyHBRecHit.iSpyHBRecHitTag = cms.InputTag('hbhereco')
process.ISpyHERecHit.iSpyHERecHitTag = cms.InputTag('hbhereco')
process.ISpyHFRecHit.iSpyHFRecHitTag = cms.InputTag('hfreco')
process.ISpyHORecHit.iSpyHORecHitTag = cms.InputTag('horeco')

process.ISpyMuon.iSpyMuonTag = cms.InputTag("muons")

process.iSpy = cms.Path(process.ISpyEvent*
                        process.ISpyTrack*
                        process.ISpyMuon*
                        process.ISpyEBRecHit*
                        process.ISpyEERecHit*
                        process.ISpyESRecHit*
                        process.ISpyHBRecHit*
                        process.ISpyHERecHit*
                        process.ISpyHFRecHit*
                        process.ISpyHORecHit)
                      
process.schedule = cms.Schedule(process.iSpy)


 
