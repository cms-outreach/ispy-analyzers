import FWCore.ParameterSet.Config as cms

ISpyL1GlobalTriggerReadoutRecord = cms.EDAnalyzer('ISpyL1GlobalTriggerReadoutRecord',
                                                  iSpyL1GlobalTriggerReadoutRecordTag = cms.InputTag("gtDigis")
                                                  )
