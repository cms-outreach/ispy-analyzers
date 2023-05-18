import FWCore.ParameterSet.Config as cms

ISpyPFMET = cms.EDAnalyzer('ISpyPFMET',
                            iSpyPFMETTag = cms.InputTag('pfMet'),
                            useCustomVector = cms.bool(False),
                            MET_phi = cms.double(0.0),
                            MET_pt = cms.double(0.0),
                            MET_px = cms.double(0.0),
                            MET_py = cms.double(0.0),
                            MET_pz = cms.double(0.0)
                           )
