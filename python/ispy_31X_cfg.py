import FWCore.ParameterSet.Config as cms

process = cms.Process("IGUANA")
process.load("ISpy/Analyzers/ISpy_Producer_cff")
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")
process.load("Configuration.StandardSequences.DigiToRaw_cff")
process.load("Configuration.StandardSequences.RawToDigi_cff")
process.load('Configuration.StandardSequences.VtxSmearedEarly10TeVCollision_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.GlobalTag.globaltag = 'MC_31X_V1::All'

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring('/store/relval/CMSSW_3_1_2/RelValH130GGgluonfusion/GEN-SIM-RECO/STARTUP31X_V2-v1/0007/FC6C2AF4-E278-DE11-B2D1-001D09F23A07.root')
)

from FWCore.MessageLogger.MessageLogger_cfi import *

process.add_(
    cms.Service("ISpyService",
    outputFileName = cms.untracked.string('RelValH130GGgluonfusion.ig')
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10)
)

process.p3 = cms.Path(process.RawToDigi)
process.p4 = cms.Path(process.reconstruction)
process.p5 = cms.Path(process.iSpy_sequence)

process.schedule = cms.Schedule(process.p3,process.p4,process.p5)
