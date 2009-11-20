import FWCore.ParameterSet.Config as cms

process = cms.Process("IGUANA")

process.load("Configuration.StandardSequences.Geometry_cff")
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('Configuration.StandardSequences.ReconstructionCosmics_cff')
process.load('Configuration.StandardSequences.VtxSmearedEarly10TeVCollision_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.GlobalTag.globaltag = "GR09_P_V4::All"
process.load('L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMenuConfig_cff')
process.es_prefer_l1GtParameters = cms.ESPrefer('L1GtTriggerMenuXmlProducer','l1GtTriggerMenuXml')
process.load('L1TriggerConfig.L1GtConfigProducers.Luminosity.lumi1031.L1Menu_MC2009_v2_L1T_Scales_20080922_Imp0_Unprescaled_cff')

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257655752.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257650306.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257652824.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257640961.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257656515.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257642699.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257653777.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257652003.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257648812.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257641879.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257655752.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257639586.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257646779.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257651271.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257648125.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257645539.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257649632.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257643602.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257633692.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257644907.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257644266.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257631602.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257626026.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257647548.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257632731.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257646212.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257623515.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257638591.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257632186.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257631058.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257655202.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257640341.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257630016.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257654646.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257629494.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257630544.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257627444.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257624089.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257629211.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257639150.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257657389.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257623101.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257627945.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257622684.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257628748.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257626657.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257628381.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257633301.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257625686.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257634399.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257627091.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257657818.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257603813.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257607712.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257617463.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257608767.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257621253.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257605052.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257606559.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257606959.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257619783.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257609132.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257610936.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257613106.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257614912.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257621955.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257624604.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257605881.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257617083.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257605518.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257609520.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257610221.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257611324.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257612390.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257613495.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257615302.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257616719.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257617822.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257620537.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257637550.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257607361.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257608416.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257610586.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257611689.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257612040.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257612754.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257613858.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257614211.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257614562.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257615665.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257616369.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257620186.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257620902.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257621604.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257624995.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257616016.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257637199.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257608077.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257609883.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257622345.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257625346.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257637913.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257638252.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257619442.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257618201.root',
'file:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_GLOBAL/malgeri/cmssw/CMSSW_3_3_2/src/DPGAnalysis/Skims/python/BS/BeamSplash_1257655752.root'
)
)

from FWCore.MessageLogger.MessageLogger_cfi import *

process.add_(
    cms.Service("ISpyService",
    outputFileName = cms.untracked.string('BeamSplash_online.ig'),
    outputMaxEvents = cms.untracked.int32(2),
    online = cms.untracked.bool(True),
    debug = cms.untracked.bool(True)
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

#process.load("ISpy.Analyzers.ISpyEventSetup_cfi")
process.load("ISpy.Analyzers.ISpyEvent_cfi")
process.load("ISpy.Analyzers.ISpyBasicCluster_cfi")
process.load("ISpy.Analyzers.ISpyCSCSegment_cfi")
process.load("ISpy.Analyzers.ISpyCSCStripDigi_cfi")
process.load("ISpy.Analyzers.ISpyCSCWireDigi_cfi")
process.load("ISpy.Analyzers.ISpyCaloTower_cfi")
process.load('ISpy.Analyzers.ISpyDTDigi_cfi')
process.load('ISpy.Analyzers.ISpyDTRecHit_cfi')
process.load("ISpy.Analyzers.ISpyDTRecSegment4D_cfi")
process.load("ISpy.Analyzers.ISpyEBRecHit_cfi")
process.load("ISpy.Analyzers.ISpyEERecHit_cfi")
process.load("ISpy.Analyzers.ISpyESRecHit_cfi")
process.load("ISpy.Analyzers.ISpyHBRecHit_cfi")
process.load("ISpy.Analyzers.ISpyHERecHit_cfi")
process.load("ISpy.Analyzers.ISpyHFRecHit_cfi")
process.load("ISpy.Analyzers.ISpyHORecHit_cfi")
process.load("ISpy.Analyzers.ISpyJet_cfi")
process.load("ISpy.Analyzers.ISpyL1GlobalTriggerReadoutRecord_cfi")
process.load("ISpy.Analyzers.ISpyMET_cfi")
process.load("ISpy.Analyzers.ISpyMuon_cfi")
process.load("ISpy.Analyzers.ISpyPixelDigi_cfi")
process.load('ISpy.Analyzers.ISpyRPCRecHit_cfi')
process.load("ISpy.Analyzers.ISpySiPixelCluster_cfi")
process.load("ISpy.Analyzers.ISpySiPixelRecHit_cfi")
process.load("ISpy.Analyzers.ISpySiStripCluster_cfi")
process.load("ISpy.Analyzers.ISpySiStripDigi_cfi")
process.load("ISpy.Analyzers.ISpyTrack_cfi")
process.load("ISpy.Analyzers.ISpyTrackingRecHit_cfi")
process.load("ISpy.Analyzers.ISpyTriggerEvent_cfi")

process.ISpyCSCStripDigi.iSpyCSCStripDigiTag = cms.InputTag("muonCSCDigis:MuonCSCStripDigi")
process.ISpyCSCWireDigi.iSpyCSCWireDigiTag = cms.InputTag("muonCSCDigis:MuonCSCWireDigi")
process.ISpyDTRecHit.iSpyDTRecHitTag = cms.InputTag("dt1DRecHits")
process.ISpyRPCRecHit.iSpyRPCRecHitTag = cms.InputTag("rpcRecHits")
process.ISpyMuon.iSpyMuonTag = cms.InputTag('muons')
process.ISpySiStripDigi.iSpySiStripDigiTag = cms.InputTag('siStripDigis:ZeroSuppressed')
process.ISpyTrackingRecHit.iSpyTrackingRecHitTag = cms.InputTag('cosmicMuons')
process.ISpyTrack.iSpyTrackTags = cms.VInputTag(cms.InputTag('cosmicMuons'),cms.InputTag('cosmictrackfinderP5'),cms.InputTag('ctfWithMaterialTracksP5'))

process.iSpy = cms.Path(process.ISpyEvent*
                       #process.ISpyEventSetup*
                       process.ISpyBasicCluster*
                       process.ISpyCSCSegment*
                       process.ISpyCSCStripDigi*
                       process.ISpyCSCWireDigi*
                       process.ISpyCaloTower*
                       process.ISpyTrack*
                       process.ISpyTrackingRecHit*
                       process.ISpyDTRecSegment4D*
                       process.ISpyDTDigi*
                       process.ISpyDTRecHit*
                       process.ISpyRPCRecHit*
                       process.ISpyEBRecHit*
                       process.ISpyEERecHit*
                       process.ISpyESRecHit*
                       process.ISpyHBRecHit*
                       process.ISpyHERecHit*
                       process.ISpyHFRecHit*
                       process.ISpyHORecHit*
                       process.ISpyJet*
                       process.ISpyMET*
                       process.ISpyMuon*
                       process.ISpyPixelDigi*
                       process.ISpySiPixelCluster*
                       process.ISpySiPixelRecHit*
                       process.ISpySiStripCluster*
                       process.ISpySiStripDigi*
                       process.ISpyL1GlobalTriggerReadoutRecord*
                       process.ISpyTriggerEvent)

process.p3= cms.Path(process.RawToDigi)
#process.p4= cms.Path(process.reconstructionCosmics)
process.schedule = cms.Schedule(process.p3,process.iSpy)
