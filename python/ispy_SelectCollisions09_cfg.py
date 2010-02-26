import FWCore.ParameterSet.Config as cms

process = cms.Process("ISPY")

process.load("ISpy/Analyzers/ISpy_Producer_cff")
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')

process.GlobalTag.globaltag = 'START3X_V16C::All'

process.source = cms.Source("PoolSource",
     fileNames = cms.untracked.vstring(  
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0026/EC7E2A81-0116-DF11-BD52-002618943867.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/FCD920B9-E515-DF11-9983-003048678FE4.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/FC36F391-E215-DF11-9DB3-00261894382A.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/FAA8D397-DC15-DF11-B8B4-002618943852.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/F878F88F-E215-DF11-9F0A-00261894387A.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/F6435592-E215-DF11-BB70-0026189437EC.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/E64DAF8E-E215-DF11-883F-0026189437F0.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/E4FE2C16-E815-DF11-81FA-003048678EE2.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/E43BE9DB-E715-DF11-AF81-002618943950.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/E2A10CB6-DB15-DF11-9BF4-0026189438BC.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/E0B4A921-F415-DF11-A06F-002618943884.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/E0209BB9-E515-DF11-B9D4-002354EF3BE0.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/DADEB5BA-E415-DF11-91B4-002618B27F8A.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/D8E7D9BC-E415-DF11-9536-003048679168.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/D819E773-DA15-DF11-AC03-002618943932.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/D0F8A3BC-E415-DF11-B0C7-0026189438D9.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/CEEB97FD-E615-DF11-8105-00304867920C.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/CAFF29FC-E615-DF11-B118-003048678FE4.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/C8728192-E215-DF11-973D-0026189438AC.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/C6E70E91-E315-DF11-9697-0026189438DF.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/C609A0B9-E415-DF11-9660-002618FDA211.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/C4B17ABB-E415-DF11-91FE-003048678B30.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/C46A36B9-E515-DF11-9609-003048679296.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/C239E6F2-ED15-DF11-9AF3-003048679168.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/B6075BB4-E615-DF11-85A0-00261894393A.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/B4BCACBB-E415-DF11-8A00-002618943867.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/B24EDDB2-E615-DF11-975C-003048678A6C.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/B0D8B12B-F815-DF11-88CD-0026189437FA.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/AA3B9DF2-E315-DF11-BCF5-0026189437EC.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/A872FCDC-E715-DF11-B952-002618FDA210.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/A4BCAFB2-E615-DF11-8CA3-003048679168.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/A2CE9C91-E215-DF11-B9CA-00304867BFAE.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/94FC92BA-E415-DF11-B6BB-002618943972.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/7E7D4591-E115-DF11-A02D-003048678ED4.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/7E52ED8F-E315-DF11-9D2B-0026189438DF.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/7CE848DD-E715-DF11-95A5-0026189438FE.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/7C2244ED-E315-DF11-87E5-0026189438FC.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/787E8A75-F915-DF11-B961-00261894392F.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/78350CDF-E315-DF11-9957-003048679168.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/66A4C78F-E315-DF11-9FFA-00261894383B.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/6477E7EE-E315-DF11-928F-002618943950.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/64257E92-E215-DF11-84D6-0026189438AE.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/64151BE1-E815-DF11-A631-0026189438B3.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/62BBF52A-E915-DF11-95E1-0026189438DF.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/5EA47ABB-E415-DF11-ACD6-003048678B5E.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/5E5AA595-E315-DF11-BA32-002618943923.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/5A1545B4-E615-DF11-82BF-002618943924.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/4A44E4F3-E315-DF11-94F9-0026189437E8.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/48516C1E-EA15-DF11-9B8F-0026189438F4.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/460596F3-E315-DF11-85C2-0026189437F0.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/3EB458BA-E515-DF11-94DC-00261894398D.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/2CD0FE8F-E215-DF11-A01B-0030486791AA.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/2AA7658D-E015-DF11-B8A6-003048679296.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/28305FBD-E415-DF11-A769-002618943953.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/225FFA90-E315-DF11-8FB9-00261894385D.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/20F35990-E315-DF11-B416-003048679164.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/1C04E991-E115-DF11-92A9-002618943867.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/0E85C591-E215-DF11-93CF-0026189438F3.root',
    '/store/data/BeamCommissioning09/MinimumBias/RECO/Feb9ReReco_v2/0025/04D38329-E915-DF11-B69A-002618943877.root'),

     eventsToProcess = cms.untracked.VEventRange(
    '124022:13339735',
    '124022:13273442'
   )                        
)

from FWCore.MessageLogger.MessageLogger_cfi import *

process.add_(
    cms.Service("ISpyService",
    outputFileName = cms.untracked.string('run124022.ig'),
    outputMaxEvents = cms.untracked.int32(10),
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.p1 = cms.Path(process.iSpy_sequence)
