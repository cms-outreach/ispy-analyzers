import FWCore.ParameterSet.Config as cms

process = cms.Process("ISPY")

process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.GlobalTag.globaltag = '103X_dataRun2_HLT_v1'

process.source = cms.Source("EmptySource")

from FWCore.MessageLogger.MessageLogger_cfi import *

process.add_(
    cms.Service("ISpyService",
    outputESFileName = cms.untracked.string('calo-geometry.ig')
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)

process.load("ISpy.Analyzers.ISpyCaloGeometry_cfi")

process.iSpy = cms.Path(process.ISpyCaloGeometry)
process.schedule = cms.Schedule(process.iSpy)
