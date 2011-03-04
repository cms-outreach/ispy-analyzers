import FWCore.ParameterSet.Config as cms

process = cms.Process("ISPY")
process.load("ISpy/Analyzers/ISpy_Producer_cff")
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load("Configuration.StandardSequences.Reconstruction_cff")
process.load("Configuration.StandardSequences.DigiToRaw_cff")
process.load("Configuration.StandardSequences.RawToDigi_cff")

process.GlobalTag.globaltag = 'MC_3XY_V21::All'

process.load('ISpy.Analyzers.ISpyEBDigi_cfi')
process.load('ISpy.Analyzers.ISpyEEDigi_cfi')

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring('/store/relval/CMSSW_3_5_0/RelValSingleElectronPt35/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_3XY_V21-v1/0013/EC5FB54B-4B13-DF11-88E7-002618FDA210.root')
)

from FWCore.MessageLogger.MessageLogger_cfi import *

process.add_(
    cms.Service("ISpyService",
    outputFileName = cms.untracked.string('ecaldigi.ig'),
    outputIg = cms.untracked.bool(True),
    outputMaxEvents = cms.untracked.int32(10),
    debug = cms.untracked.bool(True)

    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10)
)

process.iSpy_digis = cms.Path(process.ISpyEBDigi*
                              process.ISpyEEDigi)

process.p3 = cms.Path(process.RawToDigi)
process.p4 = cms.Path(process.reconstruction)
process.p5 = cms.Path(process.iSpy_sequence)
process.schedule = cms.Schedule(process.p3, process.p4, process.iSpy_digis, process.p5)
