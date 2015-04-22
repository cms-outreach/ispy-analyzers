import FWCore.ParameterSet.Config as cms

process = cms.Process("ISPY")
process.load("ISpy/Analyzers/ISpy_Producer_cff")
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.GlobalTag.globaltag = 'GR_E_V43::All' 

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
    'file:/afs/cern.ch/user/o/olivito/public/fireworks/run239754_ls0010_streamEvDOutput_dqmcluster.root',
    'file:/afs/cern.ch/user/o/olivito/public/fireworks/run239754_ls0013_streamEvDOutput_dqmcluster.root',
    'file:/afs/cern.ch/user/o/olivito/public/fireworks/run239754_ls0014_streamEvDOutput_dqmcluster.root'
    ),
    eventsToProcess = cms.untracked.VEventRange(
    '239754:128',
    '239754:138', 
    '239754:144',
    '239754:162')
)
 
from FWCore.MessageLogger.MessageLogger_cfi import *

process.ISpyCSCStripDigi.iSpyCSCStripDigiTag = cms.InputTag("muonCSCDigis:MuonCSCStripDigi")
process.ISpyCSCWireDigi.iSpyCSCWireDigiTag = cms.InputTag("muonCSCDigis:MuonCSCWireDigi")
process.ISpyDTRecHit.iSpyDTRecHitTag = cms.InputTag("dt1DRecHits")
process.ISpyRPCRecHit.iSpyRPCRecHitTag = cms.InputTag("rpcRecHits")
process.ISpyMuon.iSpyMuonTag = cms.InputTag('muons')
process.ISpySiStripDigi.iSpySiStripDigiTag = cms.InputTag('siStripDigis:ZeroSuppressed')
process.ISpyTrackingRecHit.iSpyTrackingRecHitTag = cms.InputTag('cosmicMuons')
process.ISpyTrack.iSpyTrackTags = cms.VInputTag(cms.InputTag('cosmicMuons'),cms.InputTag('cosmictrackfinderP5'),cms.InputTag('ctfWithMaterialTracksP5'))

#process.ISpyTrackingRecHit.iSpyTrackingRecHitTags = cms.VInputTag(cms.InputTag('regionalCosmicTracks'))
#process.ISpyTrack.iSpyTrackTags = cms.VInputTag(cms.InputTag('regionalCosmicTracks'))

process.add_(
    cms.Service("ISpyService",
    outputFileName = cms.untracked.string('splash2015.ig'),
    outputMaxEvents = cms.untracked.int32(100)
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.p1 = cms.Path(process.iSpy_sequence)
