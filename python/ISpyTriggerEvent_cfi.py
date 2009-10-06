import FWCore.ParameterSet.Config as cms

ISpyTriggerEvent = cms.EDAnalyzer('ISpyTriggerEvent',
                                  triggerEventTag = cms.InputTag('hltTriggerSummaryAOD'),
                                  triggerResultsTag = cms.InputTag('TriggerResults'),
                                  processName = cms.string('HLT')
                                  )
