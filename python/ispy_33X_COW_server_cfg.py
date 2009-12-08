import FWCore.ParameterSet.Config as cms
import socket

process = cms.Process("IGUANA")

process.load('Configuration/StandardSequences/Services_cff')
process.load('FWCore/MessageService/MessageLogger_cfi')
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff")
process.load('Configuration/StandardSequences/Reconstruction_cff')
process.load("Configuration.StandardSequences.RawToDigi_Data_cff")
process.load('Configuration/StandardSequences/L1Reco_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.load("ISpy/Analyzers/ISpy_Online_Producer_cff")

process.ISpyTrack.iSpyTrackTags = cms.VInputTag(cms.InputTag('generalTracks'))

process.GlobalTag.connect = "frontier://(proxyurl=http://localhost:3128)(serverurl=http://localhost:8000/FrontierOnProd)(serverurl=http://localhost:8000/FrontierOnProd)(retrieve-ziplevel=0)(failovertoserver=no)/CMS_COND_31X_GLOBALTAG"                     
process.GlobalTag.globaltag = "GR09_H_V6::All"
es_prefer_GlobalTag = cms.ESPrefer('PoolDBESSource','GlobalTag')

process.load('L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMenuConfig_cff')
process.es_prefer_l1GtParameters = cms.ESPrefer('L1GtTriggerMenuXmlProducer','l1GtTriggerMenuXml')
process.load('L1TriggerConfig.L1GtConfigProducers.Luminosity.lumi1031.L1Menu_MC2009_v2_L1T_Scales_20080922_Imp0_Unprescaled_cff')

process.source = cms.Source("EventStreamHttpReader",
   sourceURL = cms.string('http://dvsrv-c2f37-01:43100/urn:xdaq-application:lid=50'),
   consumerPriority = cms.untracked.string('normal'),
   max_event_size = cms.int32(7000000),
   consumerName = cms.untracked.string('COW iSpy Source'),
   SelectHLTOutput = cms.untracked.string('outA'),
   max_queue_depth = cms.int32(5),
   maxEventRequestRate = cms.untracked.double(10.0),
   SelectEvents = cms.untracked.PSet(
       SelectEvents = cms.vstring('*')
   ),
   headerRetryInterval = cms.untracked.int32(3)
)

from FWCore.MessageLogger.MessageLogger_cfi import *

process.add_(
    cms.Service("ISpyService",
    outputFileName = cms.untracked.string('CowServer.ig'),
    outputMaxEvents = cms.untracked.int32 (100),
    online = cms.untracked.bool(True),
    debug = cms.untracked.bool(True)
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.p= cms.Path(process.iSpy_online_sequence)
