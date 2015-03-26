import FWCore.ParameterSet.Config as cms

process = cms.Process("ISPY")

# For miniAOD
process.load("ISpy/Analyzers/ISpy_miniAOD_Producer_cff")

# For AOD
#process.load("ISpy/Analyzers/ISpy_AOD_Producer_cff")

# For RECO
#process.load("ISpy/Analyzers/ISpy_Producer_cff")

process.load("Configuration.StandardSequences.MagneticField_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('Configuration.Geometry.GeometryIdeal_cff')

from Configuration.AlCa.autoCond import autoCond
process.GlobalTag.globaltag = autoCond['mc']

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring('/store/relval/CMSSW_7_4_0_pre8/MinimumBias/MINIAOD/GR_R_74_V8A_RelVal_mb2011A-v1/00000/009E130D-52BD-E411-8C6F-0025905A60D6.root')
                            )

from FWCore.MessageLogger.MessageLogger_cfi import *

process.add_(
    cms.Service("ISpyService",
    outputFileName = cms.untracked.string('RelVal.ig'),
    outputMaxEvents = cms.untracked.int32(10)
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10)
)

process.p1 = cms.Path(process.iSpy_sequence)
