import FWCore.ParameterSet.Config as cms

process = cms.Process('ISPY')

process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('TrackingTools.TransientTrack.TransientTrackBuilder_cfi')

process.GlobalTag.globaltag = '102X_upgrade2018_realistic_v11'

process.source = cms.Source('PoolSource',
                            fileNames = cms.untracked.vstring(
    'root://cmsxrootd.fnal.gov//store/mc/RunIIAutumn18MiniAOD/gluinoGMSB_M2400_ctau30p0_TuneCP2_13TeV_pythia8/MINIAODSIM/102X_upgrade2018_realistic_v15-v1/00000/AD1ED1B2-A72D-6940-91EB-DF3124182F5E.root'

#'root://cmsxrootd.fnal.gov//store/mc/RunIIAutumn18DRPremix/gluinoGMSB_M2400_ctau30p0_TuneCP2_13TeV_pythia8/AODSIM/102X_upgrade2018_realistic_v15-v1/00000/3157D120-6A4C-7F4A-89EB-CBB87266C0C6.root'

    ))
  
from FWCore.MessageLogger.MessageLogger_cfi import *

process.add_(
    cms.Service('ISpyService',
    outputFileName = cms.untracked.string('packedCandidate.ig'),
    outputIg = cms.untracked.bool(True),
    outputMaxEvents = cms.untracked.int32(25), # These are the number of events per ig file 
    debug = cms.untracked.bool(False)
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(25) # These are the number of events to cycle through in the input root file
)

process.load('ISpy.Analyzers.ISpyEvent_cfi')
process.load('ISpy.Analyzers.ISpyEBRecHit_cfi')
process.load('ISpy.Analyzers.ISpyEERecHit_cfi')
process.load('ISpy.Analyzers.ISpyESRecHit_cfi')

process.load('ISpy.Analyzers.ISpyGenJet_cfi')

process.load('ISpy.Analyzers.ISpyPATMuon_cfi')
process.load('ISpy.Analyzers.ISpyPATElectron_cfi')
process.load('ISpy.Analyzers.ISpyPATJet_cfi')
process.load('ISpy.Analyzers.ISpyPATMET_cfi')
process.load('ISpy.Analyzers.ISpyPATPhoton_cfi')

process.load('ISpy.Analyzers.ISpyPackedCandidate_cfi')
process.load('ISpy.Analyzers.ISpyVertex_cfi')

process.ISpyEBRecHit.iSpyEBRecHitTag = cms.InputTag('reducedEgamma:reducedEBRecHits')
process.ISpyEERecHit.iSpyEERecHitTag = cms.InputTag('reducedEgamma:reducedEERecHits')
process.ISpyESRecHit.iSpyESRecHitTag = cms.InputTag('reducedEgamma:reducedESRecHits')

process.ISpyPATElectron.iSpyPATElectronTag = cms.InputTag('slimmedElectrons')
process.ISpyPATElectron.isAOD = cms.untracked.bool(True)

process.ISpyPackedCandidate.iSpyPackedCandidateTag = cms.InputTag('packedPFCandidates')
                                     
process.ISpyGenJet.iSpyGenJetTag = cms.InputTag('slimmedGenJetsAK8SoftDropSubJets')   
#process.ISpyPATJet.iSpyPATJetTag = cms.InputTag('slimmedJetsAK8')
process.ISpyPATJet.iSpyPATJetTag = cms.InputTag('slimmedJetsAK8PFPuppiSoftDropPacked:SubJets')
process.ISpyPATMET.iSpyPATMETTag = cms.InputTag('slimmedMETs')

process.ISpyPATMuon.iSpyPATMuonTag = cms.InputTag("slimmedMuons")
process.ISpyPATMuon.isAOD = cms.untracked.bool(True)

process.ISpyPATPhoton.iSpyPATPhotonTag = cms.InputTag('slimmedPhotons')

process.ISpyVertex.iSpyPriVertexTag = cms.InputTag('offlineSlimmedPrimaryVertices')

process.iSpy = cms.Path(process.ISpyEvent*
                        process.ISpyEvent*
                        process.ISpyEBRecHit*
                        process.ISpyEERecHit*
                        process.ISpyESRecHit*
                        process.ISpyPATElectron*
                        process.ISpyPATJet*
                        process.ISpyPATMET*
                        #process.ISpyPATMuon*
                        process.ISpyPATPhoton*
                        process.ISpyPackedCandidate*
                        process.ISpyVertex)

process.schedule = cms.Schedule(process.iSpy)

