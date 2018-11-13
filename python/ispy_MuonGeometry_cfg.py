import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras
process = cms.Process('ISPY',eras.Run2_2018)
#process = cms.Process('ISPY',eras.Phase2)

#process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load('Configuration.Geometry.GeometryExtended2023D21Reco_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase1_2018_realistic', '')
#process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase2_realistic', '')

process.source = cms.Source("EmptySource")

from FWCore.MessageLogger.MessageLogger_cfi import *

process.add_(
    cms.Service("ISpyService",
    outputESFileName = cms.untracked.string('muon-geometry.ig')
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)

process.load("ISpy.Analyzers.ISpyMuonGeometry_cfi")

process.iSpy = cms.Path(process.ISpyMuonGeometry)
process.schedule = cms.Schedule(process.iSpy)
