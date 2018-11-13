#ifndef ANALYZER_ISPY_TRACKER_GEOMETRY_H
#define ANALYZER_ISPY_TRACKER_GEOMETRY_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ESWatcher.h"
#include "DataFormats/DetId/interface/DetId.h"

#include <string>

class GeomDet;
class SbVec3f;
class GlobalTrackingGeometry;
class TrackerGeometry;
class IgDataStorage;
class IgCollectionItem;
class GlobalTrackingGeometryRecord;
class TrackerDigiGeometryRecord;
class TrackerTopology;

class ISpyTrackerGeometry : public edm::EDAnalyzer
{
public:
  explicit ISpyTrackerGeometry(const edm::ParameterSet&);
  virtual ~ISpyTrackerGeometry(void) {}
       
  virtual void analyze(const edm::Event&, const edm::EventSetup&);

private:
  void buildTracker3D(IgDataStorage *);
  void buildPixelBarrel3D(IgDataStorage *);
  void buildPixelEndcap3D(IgDataStorage *);
  void buildPixelEndcapMinus3D(IgDataStorage *);
  void buildPixelEndcapPlus3D(IgDataStorage *);

  void buildTIB3D(IgDataStorage *);
  void buildTOB3D(IgDataStorage *);

  void buildTEC3D(IgDataStorage *);
  void buildTECPlus3D(IgDataStorage *);
  void buildTECMinus3D(IgDataStorage *);

  void buildTID3D(IgDataStorage *);
  void buildTIDPlus3D(IgDataStorage*);
  void buildTIDMinus3D(IgDataStorage*);

  void	buildTrackerRPhi(IgDataStorage *);
  void	buildTrackerRZ(IgDataStorage *);

  bool globalTrackingGeomChanged_;
  bool trackerGeomChanged_;

  edm::ESHandle<GlobalTrackingGeometry> globalTrackingGeom_;
  edm::ESHandle<TrackerGeometry>      	trackerGeom_;
  edm::ESHandle<TrackerTopology>        trackerTopology_;

  edm::ESWatcher<GlobalTrackingGeometryRecord>  watch_globalTrackingGeom_;
  edm::ESWatcher<TrackerDigiGeometryRecord>	watch_trackerGeom_;

};

#endif // ANALYZER_ISPY_TRACKER_GEOMETRY_H
