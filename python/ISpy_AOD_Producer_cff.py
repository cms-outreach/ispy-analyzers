from ISpy.Analyzers.ISpyEvent_cfi import *

from ISpy.Analyzers.ISpyBeamSpot_cfi import *

from ISpy.Analyzers.ISpyCaloCluster_cfi import *

from ISpy.Analyzers.ISpyCaloTower_cfi import *
from ISpy.Analyzers.ISpyCaloTau_cfi import *
from ISpy.Analyzers.ISpyCaloMET_cfi import *


from ISpy.Analyzers.ISpyEBRecHit_cfi import *
from ISpy.Analyzers.ISpyEERecHit_cfi import *
from ISpy.Analyzers.ISpyESRecHit_cfi import *

from ISpy.Analyzers.ISpyEBDigi_cfi import *
from ISpy.Analyzers.ISpyEEDigi_cfi import *

from ISpy.Analyzers.ISpyElectron_cfi import *
from ISpy.Analyzers.ISpyGsfElectron_cfi import *
from ISpy.Analyzers.ISpyGsfPFRecTrack_cfi import *
from ISpy.Analyzers.ISpyGsfTrack_cfi import *

from ISpy.Analyzers.ISpyHBRecHit_cfi import *
from ISpy.Analyzers.ISpyHERecHit_cfi import *
from ISpy.Analyzers.ISpyHFRecHit_cfi import *
from ISpy.Analyzers.ISpyHORecHit_cfi import *

from ISpy.Analyzers.ISpyJet_cfi import *
from ISpy.Analyzers.ISpyL1GlobalTriggerReadoutRecord_cfi import *
from ISpy.Analyzers.ISpyMET_cfi import *
from ISpy.Analyzers.ISpyMuon_cfi import *
from ISpy.Analyzers.ISpyPATElectron_cfi import *
from ISpy.Analyzers.ISpyPATMuon_cfi import *
from ISpy.Analyzers.ISpyPATPhoton_cfi import *
from ISpy.Analyzers.ISpyPFCluster_cfi import *
from ISpy.Analyzers.ISpyPFRecHit_cfi import *
from ISpy.Analyzers.ISpyPFRecTrack_cfi import *
from ISpy.Analyzers.ISpyPFJet_cfi import *
from ISpy.Analyzers.ISpyPFMET_cfi import *
from ISpy.Analyzers.ISpyPFTau_cfi import *
from ISpy.Analyzers.ISpyPhoton_cfi import *
from ISpy.Analyzers.ISpyPixelDigi_cfi import *
from ISpy.Analyzers.ISpyPreshowerCluster_cfi import *
from ISpy.Analyzers.ISpyRPCRecHit_cfi import *



from ISpy.Analyzers.ISpySuperCluster_cfi import *
from ISpy.Analyzers.ISpyTrack_cfi import *
from ISpy.Analyzers.ISpyTrackingParticle_cfi import *
from ISpy.Analyzers.ISpyTrackingRecHit_cfi import *
from ISpy.Analyzers.ISpyTriggerEvent_cfi import *
from ISpy.Analyzers.ISpyVertex_cfi import *
from ISpy.Analyzers.ISpyTrackExtrapolation_cfi import *

ISpyEBRecHit.iSpyEBRecHitTag = cms.InputTag('reducedEcalRecHitsEB')
ISpyEERecHit.iSpyEERecHitTag = cms.InputTag('reducedEcalRecHitsEE')
ISpyESRecHit.iSpyESRecHitTag = cms.InputTag('reducedEcalRecHitsES')

ISpyHBRecHit.iSpyHBRecHitTag = cms.InputTag("reducedHcalRecHits:hbhereco")
ISpyHERecHit.iSpyHERecHitTag = cms.InputTag("reducedHcalRecHits:hbhereco")
ISpyHFRecHit.iSpyHFRecHitTag = cms.InputTag("reducedHcalRecHits:hfreco")
ISpyHORecHit.iSpyHORecHitTag = cms.InputTag("reducedHcalRecHits:horeco")

ISpyJet.iSpyJetTag = cms.InputTag("ak7CaloJets")

ISpySuperCluster.iSpySuperClusterTag = cms.InputTag("correctedHybridSuperClusters")

iSpy_sequence = cms.Sequence(ISpyEvent*
                             ISpyBeamSpot*
                             ISpyCaloCluster*
                             #ISpyCaloHit*
                             ISpyCaloTower*
                             ISpyCaloMET*
                             ISpyCaloTau*
                             ISpyEBRecHit*
                             ISpyEERecHit*
                             ISpyESRecHit*
                             ISpyEBDigi*
                             ISpyEEDigi*
                             #ISpyElectron*
                             #ISpyGsfElectron*
                             #ISpyGsfTrack*
                             ISpyHBRecHit*
                             ISpyHERecHit*
                             ISpyHFRecHit*
                             ISpyHORecHit*
                             ISpyJet*
                             ISpyMET*
                             ISpyMuon*
                             ISpyPhoton*
                             ISpySuperCluster*
                             #ISpyTrack*
                             ISpyTrackExtrapolation*
                             ISpyTriggerEvent*
                             ISpyVertex)
