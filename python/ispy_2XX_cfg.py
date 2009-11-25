import FWCore.ParameterSet.Config as cms
from FWCore.MessageLogger.MessageLogger_cfi import *

process = cms.Process("IGUANA")
process.load("ISpy/Analyzers/ISpy_Producer_cff")
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")
process.load("Configuration/StandardSequences/FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = 'STARTUP_V5::All'

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('/store/relval/CMSSW_2_2_6/RelValHiggs200ChargedTaus/GEN-SIM-RECO/STARTUP_V9_v1/0001/EC69EE50-7F0B-DE11-9FED-000423D94534.root')
)

process.add_(
    cms.Service("ISpyService",
    outputFileName = cms.untracked.string('RelValHiggs200ChargedTaus.ig')
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10)
)

process.p1 = cms.Path(process.iSpy_sequence)
