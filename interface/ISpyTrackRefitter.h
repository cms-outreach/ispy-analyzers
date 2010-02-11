#ifndef ANALYZER_ISPY_TRACKREFITTER_H
#define ANALYZER_ISPY_TRACKREFITTER_H

#include "DataFormats/TrackReco/interface/TrackFwd.h"

class IgCollectionItem;
class IgAssociations;
class IgDataStorage;
class MagneticField;

class ISpyTrackRefitter
{
public:
  static void refitTrack(IgCollectionItem& item, 
                         IgAssociations& assoc,
                         IgDataStorage* storage,
                         reco::TrackRef track, 
                         const MagneticField* field,
                         double in, double out, double step);
};

#endif // ANALYZER_ISPY_TRACKREFITTER_H
