import FWCore.ParameterSet.Config as cms
import FWCore.PythonUtilities.LumiList as LumiList

process = cms.Process("ISPY")
#goodJSON = 'Cert_190456-208686_8TeV_22Jan2013ReReco_Collisions12_JSON.txt'
#myLumis = LumiList.LumiList(filename = goodJSON).getCMSSWString().split(',')

process.load('Configuration.Geometry.GeometryIdeal_cff')
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.GlobalTag.globaltag = '76X_dataRun2_16Dec2015_v0'

# The files below can be changed to anything included in the release
process.source = cms.Source("PoolSource", fileNames = cms.untracked.vstring(

	#'root://eospublic.cern.ch//eos/opendata/cms/Run2015D/BTagCSV/AOD/16Dec2015-v1/50000/FEB7FC85-4FAE-E511-8CF1-0CC47A4D76A2.root',
	#'root://eospublic.cern.ch//eos/opendata/cms/Run2015D/BTagMu/AOD/16Dec2015-v1/60000/2C2C0169-DBBB-E511-BC2E-0025904C5182.root',
	#'root://eospublic.cern.ch//eos/opendata/cms/Run2015D/Charmonium/AOD/16Dec2015-v1/50000/FEEF54AC-75AE-E511-8BAE-002590747E14.root',
	#'root://eospublic.cern.ch//eos/opendata/cms/Run2015D/DisplacedJet/AOD/16Dec2015-v1/60000/2CEB447D-23BC-E511-9B6C-3417EBE64762.root',
	#'root://eospublic.cern.ch//eos/opendata/cms/Run2015D/DoubleEG/AOD/08Jun2016-v1/10000/A006248E-172F-E611-A39B-000F530E4BD4.root',
	#'root://eospublic.cern.ch//eos/opendata/cms/Run2015D/DoubleMuon/AOD/16Dec2015-v1/10000/AEFBFD41-6EA7-E511-9488-0CC47A4D76D0.root',
	#'root://eospublic.cern.ch//eos/opendata/cms/Run2015D/DoubleMuonLowMass/AOD/16Dec2015-v1/60000/88C60A07-82B3-E511-AA39-0025905A60AA.root',
	#'root://eospublic.cern.ch//eos/opendata/cms/Run2015D/HTMHT/AOD/16Dec2015-v1/00000/A0827A24-32B6-E511-8670-C4346BC70EC8.root',
	#'root://eospublic.cern.ch//eos/opendata/cms/Run2015D/JetHT/AOD/16Dec2015-v1/50000/AA2C104E-21A9-E511-8069-0CC47A4D76C6.root',
	#'root://eospublic.cern.ch//eos/opendata/cms/Run2015D/MET/AOD/16Dec2015-v1/50000/A29E460F-FBA9-E511-8DDB-0025905A48D0.root',
	#'root://eospublic.cern.ch//eos/opendata/cms/Run2015D/MuOnia/AOD/16Dec2015-v1/10000/AAB2196E-FCAF-E511-8193-0CC47A4D7628.root',
	#'root://eospublic.cern.ch//eos/opendata/cms/Run2015D/MuonEG/AOD/16Dec2015-v1/60000/AAEE7603-A6AA-E511-9E86-00266CF1027C.root',
	#'root://eospublic.cern.ch//eos/opendata/cms/Run2015D/SingleElectron/AOD/08Jun2016-v1/20000/A6084CDA-1438-E611-A679-0025905A60A6.root',
	#'root://eospublic.cern.ch//eos/opendata/cms/Run2015D/SingleMuon/AOD/16Dec2015-v1/10000/AA25691B-7BA8-E511-AF0F-0025905A60BE.root',
	'root://eospublic.cern.ch//eos/opendata/cms/Run2015D/SinglePhoton/AOD/16Dec2015-v1/10000/AAFEBF6C-F5A7-E511-9732-0025905B85A2.root',
	#'root://eospublic.cern.ch//eos/opendata/cms/Run2015D/Tau/AOD/16Dec2015-v1/50000/AA113152-80B3-E511-98AB-0CC47A4D7666.root',
		
))

#process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange()
#process.source.lumisToProcess.extend(myLumis)

from FWCore.MessageLogger.MessageLogger_cfi import *

# outputFileName, as the name implies, is the name of the event display file produced.
# outputMaxEvents is the number of events produced in each file.
# Actually in the file name the text "_N" will be added before the ".ig"
# For example, if maxEvents (see below) is 100, and outputMaxEvents is 25, then 4 files
# will be produced: Run2012B_0.ig, Run2012B_1.ig, Run2012B_2.ig, and Run2012B_3.ig

process.add_(
    cms.Service("ISpyService",
                outputFileName = cms.untracked.string('SinglePhoton_Run2015D.ig'),
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

process.load('ISpy.Analyzers.ISpyPFJet_cfi')
process.load('ISpy.Analyzers.ISpyPFMET_cfi')

process.load('ISpy.Analyzers.ISpyMuon_cfi')
process.load('ISpy.Analyzers.ISpyPhoton_cfi')
process.load('ISpy.Analyzers.ISpySuperCluster_cfi')

process.load('ISpy.Analyzers.ISpyTrackExtrapolation_cfi')

process.load('ISpy.Analyzers.ISpyVertex_cfi')

process.ISpyEBRecHit.iSpyEBRecHitTag = cms.InputTag('reducedEcalRecHitsEB')
process.ISpyEERecHit.iSpyEERecHitTag = cms.InputTag('reducedEcalRecHitsEE')
process.ISpyESRecHit.iSpyESRecHitTag = cms.InputTag('reducedEcalRecHitsES')

process.ISpyHBRecHit.iSpyHBRecHitTag = cms.InputTag('reducedHcalRecHits:hbhereco')
process.ISpyHERecHit.iSpyHERecHitTag = cms.InputTag('reducedHcalRecHits:hbhereco')
process.ISpyHFRecHit.iSpyHFRecHitTag = cms.InputTag('reducedHcalRecHits:hfreco')
process.ISpyHORecHit.iSpyHORecHitTag = cms.InputTag('reducedHcalRecHits:horeco')

process.ISpyMuon.iSpyMuonTag = cms.InputTag("muons")

#process.ISpyPFJet.iSpyPFJetTag = cms.InputTag('ak4PFJets')
process.ISpyPFJet.iSpyPFJetTag = cms.InputTag('pfJetsEI')

process.ISpyPFMET.iSpyPFMETTag = cms.InputTag('pfMet')
process.ISpyPhoton.iSpyPhotonTag = cms.InputTag('photons')
process.ISpySuperCluster.iSpySuperClusterTag = cms.InputTag('correctedHybridSuperClusters')
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
                        process.ISpyPhoton*
                        process.ISpySuperCluster*
                        process.ISpyTrackExtrapolation*
                        process.ISpyVertex)

process.schedule = cms.Schedule(process.iSpy)
