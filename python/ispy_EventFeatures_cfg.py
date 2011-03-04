import FWCore.ParameterSet.Config as cms

process = cms.Process('ISPY')

process.source = cms.Source('PoolSource',
   fileNames = cms.untracked.vstring(
    'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/EventDisplay/RootFileTempStorageArea/EVDISPSM_1269596402.root',
)
    )




process.add_(
    cms.Service('ISpyService',
    outputFileName = cms.untracked.string('EventFeatures.ig'),
    outputIg = cms.untracked.bool(True),
    outputMaxEvents = cms.untracked.int32(-1), # These are the number of events per ig file 
    debug = cms.untracked.bool(False)
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
    # These are the number of events to cycle through in the input root file
)

process.load('ISpy.Analyzers.ISpyEventFeatures_cfi')
process.ISpyEventFeatures.iSpyEventFeaturesTag = cms.InputTag('generalTracks')
process.iSpy = cms.Path(process.ISpyEventFeatures)
process.schedule = cms.Schedule(process.iSpy)
