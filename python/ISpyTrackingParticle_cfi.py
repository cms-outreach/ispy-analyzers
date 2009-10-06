import FWCore.ParameterSet.Config as cms

ISpyTrackingParticle = cms.EDAnalyzer('ISpyTrackingParticle',
                                      iSpyTrackingParticleTag = cms.InputTag("mergedtruth:MergedTrackTruth")
                                      )
