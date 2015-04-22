import FWCore.ParameterSet.Config as cms

process = cms.Process("ISPY")

process.load("Configuration.StandardSequences.Geometry_cff")
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'FT_R_42_V13A::All'

process.source = cms.Source("EmptySource")

from FWCore.MessageLogger.MessageLogger_cfi import *

process.add_(
    cms.Service("ISpyService",
    outputESFileName = cms.untracked.string('cms-geometry.v4.ig')
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)

process.load("ISpy.Analyzers.ISpyEventSetup_cfi")

process.iSpy = cms.Path(process.ISpyEventSetup)
process.schedule = cms.Schedule(process.iSpy)
