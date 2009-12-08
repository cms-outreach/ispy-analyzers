from ISpy.Analyzers.ISpyEvent_cfi import *
from ISpy.Analyzers.ISpyBasicCluster_cfi import *
from ISpy.Analyzers.ISpyCSCRecHit2D_cfi import *
from ISpy.Analyzers.ISpyCSCSegment_cfi import *
from ISpy.Analyzers.ISpyCSCStripDigi_cfi import *
from ISpy.Analyzers.ISpyCSCWireDigi_cfi import *
from ISpy.Analyzers.ISpyCaloCluster_cfi import *
from ISpy.Analyzers.ISpyCaloTower_cfi import *
from ISpy.Analyzers.ISpyDTDigi_cfi import *
from ISpy.Analyzers.ISpyDTRecHit_cfi import *
from ISpy.Analyzers.ISpyDTRecSegment4D_cfi import *
from ISpy.Analyzers.ISpyEBRecHit_cfi import *
from ISpy.Analyzers.ISpyEERecHit_cfi import *
from ISpy.Analyzers.ISpyESRecHit_cfi import *
from ISpy.Analyzers.ISpyElectron_cfi import *
from ISpy.Analyzers.ISpyGsfElectron_cfi import *
from ISpy.Analyzers.ISpyGsfTrack_cfi import *
from ISpy.Analyzers.ISpyHBRecHit_cfi import *
from ISpy.Analyzers.ISpyHERecHit_cfi import *
from ISpy.Analyzers.ISpyHFRecHit_cfi import *
from ISpy.Analyzers.ISpyHORecHit_cfi import *
from ISpy.Analyzers.ISpyJet_cfi import *
from ISpy.Analyzers.ISpyL1GlobalTriggerReadoutRecord_cfi import *
from ISpy.Analyzers.ISpyMET_cfi import *
from ISpy.Analyzers.ISpyMuon_cfi import *
from ISpy.Analyzers.ISpyPhoton_cfi import *
from ISpy.Analyzers.ISpyPixelDigi_cfi import *
from ISpy.Analyzers.ISpyPreshowerCluster_cfi import *
from ISpy.Analyzers.ISpyRPCRecHit_cfi import *
from ISpy.Analyzers.ISpySiPixelCluster_cfi import *
from ISpy.Analyzers.ISpySiPixelRecHit_cfi import *
from ISpy.Analyzers.ISpySiStripCluster_cfi import *
from ISpy.Analyzers.ISpySiStripDigi_cfi import *
from ISpy.Analyzers.ISpySuperCluster_cfi import *
from ISpy.Analyzers.ISpyTrack_cfi import *
from ISpy.Analyzers.ISpyTrackingRecHit_cfi import *
from ISpy.Analyzers.ISpyTriggerEvent_cfi import *

# These labels are needed to pick up cosmics
#
#ISpyTrackingRecHit.iSpyTrackingRecHitTags = cms.VInputTag(cms.InputTag('cosmicMuons'),cms.InputTag('cosmictrackfinderP5'),cms.InputTag('ctfWithMaterialTracksP5'))
#ISpyTrack.iSpyTrackTags = cms.VInputTag(cms.InputTag('cosmicMuons'),cms.InputTag('cosmictrackfinderP5'),cms.InputTag('ctfWithMaterialTracksP5'))

iSpy_online_sequence = cms.Sequence(ISpyEvent*
                             ISpyBasicCluster*
                             ISpyCSCRecHit2D*
                             ISpyCSCSegment*
                             ISpyCSCStripDigi*
                             ISpyCSCWireDigi*
                             ISpyCaloCluster*
                             ISpyCaloTower*
                             ISpyDTDigi*
                             ISpyDTRecHit*
                             ISpyDTRecSegment4D*
                             ISpyEBRecHit*
                             ISpyEERecHit*
                             ISpyESRecHit*
                             ISpyElectron*
                             ISpyGsfElectron*
                             ISpyGsfTrack*
                             ISpyHBRecHit*
                             ISpyHERecHit*
                             ISpyHFRecHit*
                             ISpyHORecHit*
                             ISpyJet*
                             ISpyL1GlobalTriggerReadoutRecord*
                             ISpyMET*
                             ISpyMuon*
                             ISpyPhoton*
                             ISpyPixelDigi*
                             ISpyPreshowerCluster*
                             ISpyRPCRecHit*
                             ISpySiPixelCluster*
                             ISpySiPixelRecHit*
                             ISpySiStripCluster*
                             ISpySiStripDigi*
                             ISpySuperCluster*
                             ISpyTrack*
                             ISpyTrackingRecHit*
                             ISpyTriggerEvent)
