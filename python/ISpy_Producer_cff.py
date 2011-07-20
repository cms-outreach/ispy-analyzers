from ISpy.Analyzers.ISpyEvent_cfi import *
from ISpy.Analyzers.ISpyBasicCluster_cfi import *
from ISpy.Analyzers.ISpyCSCRecHit2D_cfi import *
from ISpy.Analyzers.ISpyCSCSegment_cfi import *
from ISpy.Analyzers.ISpyCSCStripDigi_cfi import *
from ISpy.Analyzers.ISpyCSCWireDigi_cfi import *
from ISpy.Analyzers.ISpyCaloCluster_cfi import *
#from ISpy.Analyzers.ISpyCaloHit_cfi import *
from ISpy.Analyzers.ISpyCaloTower_cfi import *
from ISpy.Analyzers.ISpyCaloTau_cfi import *
from ISpy.Analyzers.ISpyCaloMET_cfi import *
from ISpy.Analyzers.ISpyDTDigi_cfi import *
from ISpy.Analyzers.ISpyDTRecHit_cfi import *
from ISpy.Analyzers.ISpyDTRecSegment4D_cfi import *
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
from ISpy.Analyzers.ISpySiPixelCluster_cfi import *
from ISpy.Analyzers.ISpySiPixelRecHit_cfi import *
from ISpy.Analyzers.ISpySiStripCluster_cfi import *
from ISpy.Analyzers.ISpySiStripDigi_cfi import *
#from ISpy.Analyzers.ISpySimTrack_cfi import *
from ISpy.Analyzers.ISpySuperCluster_cfi import *
from ISpy.Analyzers.ISpyTrack_cfi import *
from ISpy.Analyzers.ISpyTrackingParticle_cfi import *
from ISpy.Analyzers.ISpyTrackingRecHit_cfi import *
from ISpy.Analyzers.ISpyTriggerEvent_cfi import *
from ISpy.Analyzers.ISpyVertex_cfi import *

# These labels are needed to pick up cosmics
#
#ISpyTrackingRecHit.iSpyTrackingRecHitTags = cms.VInputTag(cms.InputTag('cosmicMuons'),cms.InputTag('cosmictrackfinderP5'),cms.InputTag('ctfWithMaterialTracksP5'))
#ISpyTrack.iSpyTrackTags = cms.VInputTag(cms.InputTag('cosmicMuons'),cms.InputTag('cosmictrackfinderP5'),cms.InputTag('ctfWithMaterialTracksP5'))

iSpy_sequence = cms.Sequence(ISpyEvent*
                             ISpyBasicCluster*
                             ISpyCSCRecHit2D*
                             ISpyCSCSegment*
                             ISpyCSCStripDigi*
                             ISpyCSCWireDigi*
                             ISpyCaloCluster*
                             #ISpyCaloHit*
                             ISpyCaloTower*
                             ISpyCaloMET*
                             ISpyCaloTau*
                             ISpyDTDigi*
                             ISpyDTRecHit*
                             ISpyDTRecSegment4D*
                             ISpyEBRecHit*
                             ISpyEERecHit*
                             ISpyESRecHit*
                             ISpyEBDigi*
                             ISpyEEDigi*
                             ISpyElectron*
                             ISpyGsfElectron*
                             ISpyGsfPFRecTrack*
                             ISpyGsfTrack*
                             ISpyHBRecHit*
                             ISpyHERecHit*
                             ISpyHFRecHit*
                             ISpyHORecHit*
                             ISpyJet*
                             ISpyL1GlobalTriggerReadoutRecord*
                             ISpyMET*
                             ISpyMuon*
                             ISpyPATElectron*
                             ISpyPATMuon*
                             ISpyPATPhoton*
                             #ISpyPFCluster*
                             ISpyPFRecHit*
                             ISpyPFRecTrack*
                             ISpyPFJet*
                             ISpyPFMET*
                             ISpyPFTau*
                             ISpyPhoton*
                             ISpyPixelDigi*
                             ISpyPreshowerCluster*
                             ISpyRPCRecHit*
                             ISpySiPixelCluster*
                             ISpySiPixelRecHit*
                             ISpySiStripCluster*
                             ISpySiStripDigi*
                             #ISpySimTrack*
                             ISpySuperCluster*
                             ISpyTrack*
                             ISpyTrackingParticle*
                             ISpyTrackingRecHit*
                             ISpyTriggerEvent*
                             ISpyVertex)
