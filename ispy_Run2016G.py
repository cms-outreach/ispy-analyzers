import FWCore.ParameterSet.Config as cms
import FWCore.PythonUtilities.LumiList as LumiList

goodJSON = 'Cert_271036-284044_13TeV_Legacy2016_Collisions16_JSON.txt'
myLumis = LumiList.LumiList(filename = goodJSON).getCMSSWString().split(',')

process = cms.Process('ISPY')

process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_data', '')


fp = 'root://eospublic.cern.ch//eos/opendata/cms/Run2016G/DoubleEG/MINIAOD/UL2016_MiniAODv2-v1/280000/'

files = [
    fp+'00D63158-21F0-F44C-BEEE-0FAE92A247B0.root',
    fp+'07957B60-9CAE-EE4F-B553-1EAAA0797F47.root',
    fp+'0F201C08-39BA-1A41-86C4-578708986653.root',
    fp+'14306187-68A4-7E49-924D-6FD070BE71AB.root',
    fp+'173AF7B0-B68C-4A49-9C07-D0B92EC00466.root',
    fp+'1A167F7A-DFC9-274A-ABD3-62AA9A2B4846.root',
    ]

process.source = cms.Source('PoolSource',
                            fileNames = cms.untracked.vstring(*files)
                            )

process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange()
process.source.lumisToProcess.extend(myLumis)
  
from FWCore.MessageLogger.MessageLogger_cfi import *

process.add_(
    cms.Service('ISpyService',
    outputFileName = cms.untracked.string('Run-2016G_DoubleEG.ig'),
    outputIg = cms.untracked.bool(True),
    outputMaxEvents = cms.untracked.int32(25),  
    debug = cms.untracked.bool(False)
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100) 
)


process.load("ISpy.Analyzers.ISpyEvent_cfi")

process.load('ISpy.Analyzers.ISpyEBRecHit_cfi')
process.load('ISpy.Analyzers.ISpyEERecHit_cfi')
process.load('ISpy.Analyzers.ISpyESRecHit_cfi')
process.load('ISpy.Analyzers.ISpyHBRecHit_cfi')
process.load('ISpy.Analyzers.ISpyHERecHit_cfi')
process.load('ISpy.Analyzers.ISpyHFRecHit_cfi')
process.load('ISpy.Analyzers.ISpyHORecHit_cfi')

process.load('ISpy.Analyzers.ISpyPATMuon_cfi')
process.load('ISpy.Analyzers.ISpyPATElectron_cfi')
process.load('ISpy.Analyzers.ISpyPackedCandidate_cfi')
process.load('ISpy.Analyzers.ISpyVertex_cfi')
process.load('ISpy.Analyzers.ISpyPATJet_cfi')
process.load('ISpy.Analyzers.ISpyPATMET_cfi')

process.ISpyEBRecHit.iSpyEBRecHitTag = cms.InputTag('reducedEgamma:reducedEBRecHits')
process.ISpyEERecHit.iSpyEERecHitTag = cms.InputTag('reducedEgamma:reducedEERecHits')
process.ISpyESRecHit.iSpyESRecHitTag = cms.InputTag('reducedEgamma:reducedESRecHits')

process.ISpyHBRecHit.iSpyHBRecHitTag = cms.InputTag("reducedEgamma:reducedHBHEHits")
process.ISpyHERecHit.iSpyHERecHitTag = cms.InputTag("reducedEgamma:reducedHBHEHits")

#process.ISpyHBRecHit.iSpyHBRecHitTag = cms.InputTag("slimmedHcalRecHits:reducedHcalRecHits"
#process.ISpyHERecHit.iSpyHERecHitTag = cms.InputTag("slimmedHcalRecHits:reducedHcalRecHits"

process.ISpyHFRecHit.iSpyHFRecHitTag = cms.InputTag("slimmedHcalRecHits:reducedHcalRecHits")
process.ISpyHORecHit.iSpyHORecHitTag = cms.InputTag("slimmedHcalRecHits:reducedHcalRecHits")

process.ISpyPATMuon.iSpyPATMuonTag = cms.InputTag("slimmedMuons")
process.ISpyPATElectron.ispyPATElectronTag = cms.InputTag('slimmedElectrons')

process.ISpyPackedCandidate.iSpyPackedCandidateTag = cms.InputTag('packedPFCandidates')
process.ISpyPATJet.iSpyPATJetTag = cms.InputTag('slimmedJets')
process.ISpyPATMET.iSpyPATMETTag = cms.InputTag('slimmedMETs')
process.ISpyVertex.iSpyPriVertexTag = cms.InputTag('offlineSlimmedPrimaryVertices')

process.iSpy = cms.Path(process.ISpyEvent*
                        process.ISpyEBRecHit*
                        process.ISpyEERecHit*
                        process.ISpyESRecHit*
                        process.ISpyHBRecHit*
                        process.ISpyHERecHit*
                        process.ISpyHFRecHit*
                        process.ISpyHORecHit*
                        process.ISpyPATMuon*
                        process.ISpyPATElectron*
                        process.ISpyPATJet*
                        process.ISpyPATMET*
                        process.ISpyPackedCandidate*
                        process.ISpyVertex)

process.schedule = cms.Schedule(process.iSpy)





