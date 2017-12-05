import FWCore.ParameterSet.Config as cms
import FWCore.PythonUtilities.LumiList as LumiList

process = cms.Process("ISPY")
goodJSON = 'Cert_190456-208686_8TeV_22Jan2013ReReco_Collisions12_JSON.txt'
myLumis = LumiList.LumiList(filename = goodJSON).getCMSSWString().split(',')

process.load('Configuration.Geometry.GeometryIdeal_cff')
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.GlobalTag.globaltag = 'FT_53_LV5_AN1::All'

# The files below can be changed to anything included in the release
process.source = cms.Source("PoolSource", fileNames = cms.untracked.vstring(
    'root://eospublic.cern.ch//eos/opendata/cms/Run2012B/DoubleMuParked/AOD/22Jan2013-v1/10000/1EC938EF-ABEC-E211-94E0-90E6BA442F24.root'
))

process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange()
process.source.lumisToProcess.extend(myLumis)

from FWCore.MessageLogger.MessageLogger_cfi import *

# outputFileName, as the name implies, is the name of the event display file produced.
# outputMaxEvents is the number of events produced in each file.
# Actually in the file name the text "_N" will be added before the ".ig"
# For example, if maxEvents (see below) is 100, and outputMaxEvents is 25, then 4 files
# will be produced: Run2012B_0.ig, Run2012B_1.ig, Run2012B_2.ig, and Run2012B_3.ig

process.add_(
    cms.Service("ISpyService",
                outputFileName = cms.untracked.string('DoubleMuParked_Run2012B.ig'),
                outputMaxEvents = cms.untracked.int32(25)
               )
)

# This number is the total number of events run over. "-1" indicates all.
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(25)
)

process.load('ISpy.Analyzers.ISpyEvent_cfi')

process.load('ISpy.Analyzers.ISpyEBRecHit_cfi')
process.load('ISpy.Analyzers.ISpyEERecHit_cfi')
process.load('ISpy.Analyzers.ISpyESRecHit_cfi')

process.load('ISpy.Analyzers.ISpyHBRecHit_cfi')
process.load('ISpy.Analyzers.ISpyHERecHit_cfi')
process.load('ISpy.Analyzers.ISpyHFRecHit_cfi')
process.load('ISpy.Analyzers.ISpyHORecHit_cfi')

process.load('ISpy.Analyzers.ISpyPFJet_cfi')
process.load('ISpy.Analyzers.ISpyPFMET_cfi')
process.load('ISpy.Analyzers.ISpyPFTau_cfi')

process.load('ISpy.Analyzers.ISpyMuon_cfi')
process.load('ISpy.Analyzers.ISpyPhoton_cfi')
process.load('ISpy.Analyzers.ISpySuperCluster_cfi')

process.load('ISpy.Analyzers.ISpyTrackExtrapolation_cfi')

process.load('ISpy.Analyzers.ISpyTriggerEvent_cfi')

process.load('ISpy.Analyzers.ISpyVertex_cfi')

process.ISpyEBRecHit.iSpyEBRecHitTag = cms.InputTag('reducedEcalRecHitsEB')
process.ISpyEERecHit.iSpyEERecHitTag = cms.InputTag('reducedEcalRecHitsEE')
process.ISpyESRecHit.iSpyESRecHitTag = cms.InputTag('reducedEcalRecHitsES')

process.ISpyHBRecHit.iSpyHBRecHitTag = cms.InputTag('reducedHcalRecHits:hbhereco')
process.ISpyHERecHit.iSpyHERecHitTag = cms.InputTag('reducedHcalRecHits:hbhereco')
process.ISpyHFRecHit.iSpyHFRecHitTag = cms.InputTag('reducedHcalRecHits:hfreco')
process.ISpyHORecHit.iSpyHORecHitTag = cms.InputTag('reducedHcalRecHits:horeco')

process.ISpyMuon.iSpyMuonTag = cms.InputTag("muons")

process.ISpyPFJet.iSpyPFJetTag = cms.InputTag('ak7PFJets')
process.ISpyPFMET.iSpyPFMETTag = cms.InputTag('pfMet')
process.ISpyPFTau.iSpyPFTauTag = cms.InputTag('hpsPFTauProducer')

process.ISpyPhoton.iSpyPhotonTag = cms.InputTag('photons')

process.ISpySuperCluster.iSpySuperClusterTag = cms.InputTag('correctedHybridSuperClusters')

process.ISpyTriggerEvent.triggerEventTag = cms.InputTag('hltTriggerSummaryAOD')
process.ISpyTriggerEvent.triggerResultsTag = cms.InputTag('TriggerResults')
process.ISpyTriggerEvent.processName = cms.string('HLT')

process.ISpyVertex.iSpyVertexTag = cms.InputTag('offlinePrimaryVertices')

process.iSpy = cms.Path(process.ISpyEvent*
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
                        process.ISpyPFTau*
                        process.ISpyPhoton*
                        process.ISpySuperCluster*
                        process.ISpyTrackExtrapolation*
                        process.ISpyTriggerEvent*
                        process.ISpyVertex)

process.schedule = cms.Schedule(process.iSpy)

