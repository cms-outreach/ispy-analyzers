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
    #'/store/data/Run2011A/PhotonHad/AOD/12Oct2013-v1/00000/024938EB-3445-E311-A72B-002590593920.root'
    #'/store/data/Run2011A/SingleElectron/AOD/12Oct2013-v1/10000/0EC2E6B2-1140-E311-8EEE-003048F00BB0.root'
    #'/store/data/Run2011A/SingleMu/AOD/12Oct2013-v1/10000/00082EAF-C03D-E311-8E53-003048F00B1C.root'
    #'/store/data/Run2011A/Tau/AOD/12Oct2013-v1/10000/000D943F-8B40-E311-A590-FA163EE5B451.root'
    #'/store/data/Run2011A/TauPlusX/AOD/12Oct2013-v1/10000/0013F0F9-3E43-E311-8D7F-FA163EBFD6C9.root'
    #'/store/data/Run2011A/MultiJet/AOD/12Oct2013-v1/00000/001D2AFA-8B43-E311-AC56-02163E009EC4.root'
    #'/store/data/Run2011A/Photon/AOD/12Oct2013-v1/10000/AC980FCC-943D-E311-A1D6-003048F11824.root'
    #'/store/data/Run2011A/BTag/AOD/12Oct2013-v1/00000/802CF580-BB46-E311-8D89-00261894388D.root'
    #'/store/data/Run2011A/DoubleElectron/AOD/12Oct2013-v1/20000/0014CE62-9C3E-E311-8FCC-00261894389F.root'
    #'/store/data/Run2011A/DoubleMu/AOD/12Oct2013-v1/10000/000D143E-9535-E311-B88B-002618943934.root'
    #'/store/data/Run2011A/ElectronHad/AOD/12Oct2013-v1/20000/003EB17D-D140-E311-9057-003048F1C7BC.root'

    # can't find file? '/store/data/Run2011A/ForwardTriggers/AOD/12Oct2013-v1/00000/2657E5CA-8945-E311-B334-02163E008CCD.root'

    #'/store/data/Run2011A/HT/AOD/12Oct2013-v1/00000/20E00925-AD48-E311-ADD5-0025905964B6.root'
    #'/store/data/Run2011A/Jet/AOD/12Oct2013-v1/20000/000D4260-D23E-E311-A850-02163E008D77.root'
    #'/store/data/Run2011A/MET/AOD/12Oct2013-v1/20000/000A5D59-B842-E311-B073-003048679296.root'
    #'/store/data/Run2011A/METBTag/AOD/12Oct2013-v1/00000/D639BB50-3344-E311-987E-003048679236.root'
    #'/store/data/Run2011A/MinimumBias/AOD/12Oct2013-v1/00000/000971D5-7F46-E311-A57D-02163E008F6D.root'

    # no events? '/store/data/Run2011A/MinimumBias0/AOD/12Oct2013-v1/00000/04B6A98D-1D48-E311-BBF7-003048F23FA2.root'    
    # no events? '/store/data/Run2011A/MinimumBias1/AOD/12Oct2013-v1/20000/0675F4B9-D647-E311-A9D5-02163E00CFE4.root'
    # no events? '/store/data/Run2011A/MinimumBias2/AOD/12Oct2013-v1/00000/0423BC6D-FA47-E311-AC9D-C860001BD84A.root'
    
    #'/store/data/Run2011A/MuEG/AOD/12Oct2013-v1/20000/003DD74E-653E-E311-9F63-002590494D18.root'
    #'/store/data/Run2011A/MuHad/AOD/12Oct2013-v1/00000/006A70B9-7845-E311-ADC1-003048679188.root'
    '/eos/opendata/cms/Run2011A/MuOnia/AOD/12Oct2013-v1/00000/002104A5-2A44-E311-B69F-00304867918A.root'
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
 
