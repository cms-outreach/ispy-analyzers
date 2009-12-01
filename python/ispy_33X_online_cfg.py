import FWCore.ParameterSet.Config as cms

process = cms.Process("IGUANA")

process.load('Configuration/StandardSequences/Services_cff')
process.load('FWCore/MessageService/MessageLogger_cfi')
process.load('Configuration/StandardSequences/GeometryExtended_cff')
process.load('Configuration/StandardSequences/MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration/StandardSequences/RawToDigi_Data_cff')
process.load('Configuration/StandardSequences/L1Reco_cff')
process.load('Configuration/StandardSequences/Reconstruction_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
#process.load("DQM.Integration.test.FrontierCondition_GT_cfi")
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'GR09_P_V6::All'
process.load("ISpy/Analyzers/ISpy_Online_Producer_cff")

process.source = cms.Source("NewEventStreamFileReader",
    fileNames = cms.untracked.vstring(
                                      'file:/lookarea_SM/Data.00123151.0001.Express.storageManager.00.0000.dat',
                                      'file:/lookarea_SM/Data.00123151.0008.Express.storageManager.01.0000.dat',
                                      'file:/lookarea_SM/Data.00123151.0015.Express.storageManager.02.0000.dat',
                                      'file:/lookarea_SM/Data.00123151.0022.Express.storageManager.03.0000.dat',
                                      'file:/lookarea_SM/Data.00121995.0014.Express.storageManager.03.0000.dat')
                            )

from FWCore.MessageLogger.MessageLogger_cfi import *

process.add_(
    cms.Service("ISpyService",
    outputFileName = cms.untracked.string('Collisions.30.11.09.ig'),
    outputMaxEvents = cms.untracked.int32 (100),
    outputMaxTime = cms.untracked.int32 (10),
    online = cms.untracked.bool(False),
    debug = cms.untracked.bool(True)
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.load("HLTrigger.special.HLTTriggerTypeFilter_cfi")
# 0=random, 1=physics, 2=calibration, 3=technical
process.hltTriggerTypeFilter.SelectedTriggerType = 1

process.p3= cms.Path(process.hltTriggerTypeFilter*process.RawToDigi)
process.p4= cms.Path(process.reconstruction*process.iSpy_online_sequence)
process.schedule = cms.Schedule(process.p3,process.p4)

