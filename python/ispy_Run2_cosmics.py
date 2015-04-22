import FWCore.ParameterSet.Config as cms

process = cms.Process("ISPY")
process.load("ISpy/Analyzers/ISpy_Producer_cff")
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.GlobalTag.globaltag = 'GR_E_V43::All' 

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
    '/store/data/Commissioning2015/Cosmics/RAW-RECO/CosmicSP-CosmicsSP_07Feb2015-v2/20000/BA687916-5FC1-E411-AEEA-02163E00EAC2.root'
    ),
 eventsToProcess = cms.untracked.VEventRange(
'233235:623',
'233235:11563', 
'233235:13439') )
 
from FWCore.MessageLogger.MessageLogger_cfi import *

process.ISpyTrackingRecHit.iSpyTrackingRecHitTags = cms.VInputTag(cms.InputTag('regionalCosmicTracks'))
process.ISpyTrack.iSpyTrackTags = cms.VInputTag(cms.InputTag('regionalCosmicTracks'))

process.add_(
    cms.Service("ISpyService",
    outputFileName = cms.untracked.string('cosmics.ig'),
    outputMaxEvents = cms.untracked.int32(100)
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.p1 = cms.Path(process.iSpy_sequence)
