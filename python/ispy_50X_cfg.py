import FWCore.ParameterSet.Config as cms

process = cms.Process("ISPY")
process.load("ISpy/Analyzers/ISpy_Producer_cff")
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

#from Configuration.PyReleaseValidation.autoCond import autoCond
from Configuration.AlCa.autoCond import autoCond
process.GlobalTag.globaltag = autoCond['mc']

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring('/store/relval/CMSSW_5_0_0_pre7/SingleMu/RECO/GR_R_50_V5_RelVal_wMu2011A-v1/0000/0016E4D7-1E22-E111-8472-00261894388A.root')
                            )

from FWCore.MessageLogger.MessageLogger_cfi import *

process.add_(
    cms.Service("ISpyService",
    outputFileName = cms.untracked.string('RelValSingleMu.ig'),
    outputMaxEvents = cms.untracked.int32(10)
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10)
)

process.p1 = cms.Path(process.iSpy_sequence)
