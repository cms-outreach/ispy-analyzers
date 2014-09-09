import FWCore.ParameterSet.Config as cms
import FWCore.PythonUtilities.LumiList as LumiList

process = cms.Process("ISPY")
goodJSON = 'Cert_136033-149442_7TeV_Apr21ReReco_Collisions10_JSON_v2.txt'
myLumis = LumiList.LumiList(filename = goodJSON).getCMSSWString().split(',')

process.load("ISpy/Analyzers/ISpy_AOD_Producer_cff")
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.GlobalTag.globaltag = 'GR_R_42_V25::All'

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
'root://eospublic.cern.ch//eos/opendata/cms/Run2010B/BTau/AOD/Apr21ReReco-v1/0000/00E16FBB-9071-E011-83D3-003048673F12.root',
'root://eospublic.cern.ch//eos/opendata/cms/Run2010B/Commissioning/AOD/Apr21ReReco-v1/0000/00B85831-2471-E011-AC0D-0024E8768C3D.root',
'root://eospublic.cern.ch//eos/opendata/cms/Run2010B/EGMonitor/AOD/Apr21ReReco-v1/0000/00091D4E-6170-E011-9DAD-0018F3D09616.root',
'root://eospublic.cern.ch//eos/opendata/cms/Run2010B/Electron/AOD/Apr21ReReco-v1/0000/000E4FCB-596E-E011-A0BB-00304867BF18.root',
'root://eospublic.cern.ch//eos/opendata/cms/Run2010B/Jet/AOD/Apr21ReReco-v1/0000/00052C5A-EF70-E011-B43F-00266CF32A00.root',
'root://eospublic.cern.ch//eos/opendata/cms/Run2010B/JetMETTauMonitor/AOD/Apr21ReReco-v1/0000/00418FA6-3170-E011-9D42-E0CB4E55367B.root',
'root://eospublic.cern.ch//eos/opendata/cms/Run2010B/METFwd/AOD/Apr21ReReco-v1/0000/00071EFE-0D74-E011-8F9C-0024E8768867.root',
'root://eospublic.cern.ch//eos/opendata/cms/Run2010B/MinimumBias/AOD/Apr21ReReco-v1/0000/008A07E5-1771-E011-AC8F-001A92971B62.root',
'root://eospublic.cern.ch//eos/opendata/cms/Run2010B/Mu/AOD/Apr21ReReco-v1/0000/00459D48-EB70-E011-AF09-90E6BA19A252.root',
'root://eospublic.cern.ch//eos/opendata/cms/Run2010B/MuMonitor/AOD/Apr21ReReco-v1/0000/00059300-8771-E011-9B6C-00E081791777.root',
'root://eospublic.cern.ch//eos/opendata/cms/Run2010B/MuOnia/AOD/Apr21ReReco-v1/0000/0029E804-C77C-E011-BA94-00215E22239A.root',
'root://eospublic.cern.ch//eos/opendata/cms/Run2010B/MultiJet/AOD/Apr21ReReco-v1/0000/002A8227-7471-E011-9632-002618943838.root',
'root://eospublic.cern.ch//eos/opendata/cms/Run2010B/Photon/AOD/Apr21ReReco-v1/0000/0003D1EF-B071-E011-BA0A-78E7D1651098.root'
))

process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange()
process.source.lumisToProcess.extend(myLumis)
 
from FWCore.MessageLogger.MessageLogger_cfi import *

process.add_(
    cms.Service("ISpyService",
    outputFileName = cms.untracked.string('Run2010B.ig'),
    outputMaxEvents = cms.untracked.int32(25)
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.p1 = cms.Path(process.iSpy_sequence)

