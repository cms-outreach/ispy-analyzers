import FWCore.ParameterSet.Config as cms

process = cms.Process("ISPY")

process.load("Configuration.Geometry.GeometryExtended2017Plan1Reco_cff")
process.load("Geometry.TrackerGeometryBuilder.trackerParameters_cfi")

process.load("Configuration.StandardSequences.MagneticField_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.trackerGeometry.applyAlignment = cms.bool(False)
process.GlobalTag.globaltag = '90X_dataRun2_Express_v4'

process.source = cms.Source("EmptySource")

from FWCore.MessageLogger.MessageLogger_cfi import *

process.add_(
    cms.Service("ISpyService",
    outputESFileName = cms.untracked.string('cms-geometry.v5.ig')
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)

process.load("ISpy.Analyzers.ISpyEventSetup_cfi")

process.iSpy = cms.Path(process.ISpyEventSetup)
process.schedule = cms.Schedule(process.iSpy)
