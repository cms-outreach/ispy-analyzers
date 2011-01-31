import FWCore.ParameterSet.Config as cms

process = cms.Process("ISPY")
process.load("ISpy/Analyzers/ISpy_Producer_cff")
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.GlobalTag.globaltag = 'START311_V0::All'

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring('/store/relval/CMSSW_3_11_0_pre5/RelValZMM/GEN-SIM-RECO/START311_V0_64bit-v1/0067/F8A51842-D324-E011-B8F5-00261894387D.root')
                            )

from FWCore.MessageLogger.MessageLogger_cfi import *

process.add_(
    cms.Service("ISpyService",
    outputFileName = cms.untracked.string('RelValZMM.ig'),
    outputIg = cms.untracked.bool(True),
    outputMaxEvents = cms.untracked.int32(10),
    online = cms.untracked.bool(False),
    debug = cms.untracked.bool(False)

    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10)
)

process.ISpyCaloTower.iSpyCaloTowerTag = cms.InputTag('towerMaker')

process.p1 = cms.Path(process.iSpy_sequence)
