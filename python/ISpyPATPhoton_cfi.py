import FWCore.ParameterSet.Config as cms

ISpyPATPhoton = cms.EDAnalyzer('ISpyPATPhoton',
                               iSpyPATPhotonTag = cms.InputTag('allLayer1Photons')
                               )
