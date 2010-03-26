import FWCore.ParameterSet.Config as cms

ISpyEventFeatures = cms.EDAnalyzer('ISpyEventFeatures',
                            iSpyEventFeaturesTag = cms.InputTag('generalTracks')
                                   )
