import FWCore.ParameterSet.Config as cms

process = cms.Process("IGUANA")
process.load("ISpy/Analyzers/ISpy_Producer_cff")
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')


process.GlobalTag.globaltag = 'STARTUP3X_V14::All'


process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring('/store/relval/CMSSW_3_5_0_pre2/RelValTTbar/GEN-SIM-RECO/STARTUP3X_V14-v1/0010/76A5863F-22EE-DE11-A745-002618943896.root')
)

from FWCore.MessageLogger.MessageLogger_cfi import *

process.add_(
    cms.Service("ISpyService",
    outputFileName = cms.untracked.string('RelValTTbar.ig'),
    outputIg = cms.untracked.bool(True),
    outputMaxEvents = cms.untracked.int32(10),
    online = cms.untracked.bool(True),
    debug = cms.untracked.bool(True)

    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10)
)

process.p1 = cms.Path(process.iSpy_sequence)
