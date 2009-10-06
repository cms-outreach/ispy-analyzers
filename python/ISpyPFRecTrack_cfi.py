import FWCore.ParameterSet.Config as cms

ISpyPFRecTrack = cms.EDAnalyzer('ISpyPFRecTrack' ,
                                iSpyPFRecTrackTag = cms.InputTag("trackerDrivenElectronSeeds")
                                )
