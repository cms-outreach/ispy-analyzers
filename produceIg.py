import FWCore.ParameterSet.Config as cms
import FWCore.PythonUtilities.LumiList as LumiList

process = cms.Process("ISPY")
goodJSON = 'Cert_160404-180252_7TeV_ReRecoNov08_Collisions11_JSON.txt'
myLumis = LumiList.LumiList(filename = goodJSON).getCMSSWString().split(',')

process.load("ISpy/Analyzers/ISpy_AOD_Producer_cff")
process.load('Configuration.Geometry.GeometryIdeal_cff')
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.GlobalTag.globaltag = 'FT_53_LV5_AN1::All'

# The files below can be changed to anything included in the release
process.source = cms.Source("PoolSource", fileNames = cms.untracked.vstring(
    'root://eospublic.cern.ch//eos/opendata/cms/Run2011A/MuOnia/AOD/12Oct2013-v1/00000/002104A5-2A44-E311-B69F-00304867918A.root'
))

process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange()
process.source.lumisToProcess.extend(myLumis)

from FWCore.MessageLogger.MessageLogger_cfi import *

# outputFileName, as the name implies, is the name of the event display file produced.
# outputMaxEvents is the number of events produced in each file.
# Actually in the file name the text "_N" will be added before the ".ig"
# For example, if maxEvents (see below) is 100, and outputMaxEvents is 25, then 4 files
# will be produced: Run2011A_0.ig, Run2011A_1.ig, Run2011A_2.ig, and Run2011A_3.ig
process.add_(
    cms.Service("ISpyService",
                outputFileName = cms.untracked.string('MuOnia_Run2011A.ig'),
                outputMaxEvents = cms.untracked.int32(25)
               )
)

# This number is the total number of events run over. "-1" indicates all.
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(25)
)

process.p1 = cms.Path(process.iSpy_sequence)
 
