import FWCore.ParameterSet.Config as cms

ISpyPhoton = cms.EDAnalyzer('ISpyPhoton',
                            iSpyPhotonTag = cms.InputTag('photons')
                            )
