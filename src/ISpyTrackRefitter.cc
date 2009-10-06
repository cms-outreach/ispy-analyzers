#include "ISpy/Analyzers/interface/ISpyTrackRefitter.h"
#include "TrackPropagation/SteppingHelixPropagator/interface/SteppingHelixPropagator.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "ISpy/Services/interface/IgCollection.h"
#include "DataFormats/GeometrySurface/interface/PlaneBuilder.h"

void ISpyTrackRefitter::refitTrack(IgCollectionItem& item, 
				   IgAssociationSet& association, 
				   IgDataStorage* storage, 
				   reco::TrackRef track, 
				   const MagneticField* field,
				   double in, double out, double step) 
{    
  if ( track.isNonnull() )
  {
    IgCollection &points = storage->getCollection("Points_V1");

    reco::TransientTrack aRealTrack(track, field);

    //Get a propagator and inner/outer state
    SteppingHelixPropagator propagator(field, alongMomentum);
    SteppingHelixPropagator reversePropagator(field, oppositeToMomentum);

    GlobalPoint gPin((*track).innerPosition().x(),
                     (*track).innerPosition().y(),
                     (*track).innerPosition().z());
    
    GlobalPoint gPout((*track).outerPosition().x(),
                      (*track).outerPosition().y(),
                      (*track).outerPosition().z());
    
    GlobalVector InOutVector = (gPout - gPin);

    // Define rotation for plane perpendicular to InOutVector
    // z axis coincides with perp
    GlobalVector zAxis = InOutVector.unit();

    // x axis has no global Z component
    GlobalVector xAxis;

    if (zAxis.x() != 0 || zAxis.y() != 0) 
    {
      // precision is not an issue here, just protect against divizion by zero
      xAxis = GlobalVector (-zAxis.y(), zAxis.x(), 0).unit();
    }
    
    else 
    { // perp coincides with global Z
      xAxis = GlobalVector( 1, 0, 0);
    }
    // y axis obtained by cross product
    GlobalVector yAxis( zAxis.cross( xAxis));
    
    Surface::RotationType rot(xAxis.x(), xAxis.y(), xAxis.z(),
                              yAxis.x(), yAxis.y(), yAxis.z(),
                              zAxis.x(), zAxis.y(), zAxis.z());

    // Define step size and number of extra steps on inside and outside
    int nSteps = 0;
    int nStepsInside = 0;
    int nStepsOutside = 0;

    GlobalVector StepVector;
    
    if( step > 0.01 ) 
    {
      StepVector = InOutVector * step;
      nSteps = int (0.5 + 1.0/step);
      nStepsInside = int (0.5 + in/step);
      nStepsOutside = int (0.5 + out/step);
    } 

    else 
    {
      StepVector = InOutVector * 0.01;
      nSteps = 100;
      nStepsInside = int (0.5 + in*100);
      nStepsOutside = int (0.5 + out*100);	  
    } 

    GlobalVector gVin((*track).innerMomentum ().x(),
                      (*track).innerMomentum ().y(),
                      (*track).innerMomentum ().z());
    
    GlobalVector gVout((*track).outerMomentum ().x(),
                       (*track).outerMomentum ().y(),
                       (*track).outerMomentum ().z());
	
    GlobalTrajectoryParameters GTPin (gPin, gVin, aRealTrack.impactPointTSCP ().charge (), field);
    FreeTrajectoryState FTSin (GTPin);

    GlobalTrajectoryParameters GTPout (gPout, gVout, aRealTrack.impactPointTSCP ().charge (), field);
    FreeTrajectoryState FTSout (GTPout);

    int nGood = 0;
    int nBad = 0;

    // Do nStepsInside propagations on inside to plot track
    GlobalPoint GP = gPin; 
    GP -= nStepsInside * StepVector;
    Basic3DVector<float> Basic3DV (StepVector.x (), StepVector.y (), StepVector.z ());
	
    for (int istep = 0; istep < nStepsInside; ++istep) 
    {
      GP += StepVector;
      Surface::PositionType pos (GP.x (), GP.y (), GP.z ());
      PlaneBuilder::ReturnType SteppingPlane = PlaneBuilder ().plane (pos, rot);
      TrajectoryStateOnSurface trj =  reversePropagator.propagate (FTSin, *SteppingPlane);
	
      if (trj.isValid ())
      {
        float x = trj.globalPosition ().x () / 100.0;
        float y = trj.globalPosition ().y () / 100.0;
        float z = trj.globalPosition ().z () / 100.0;		   
        
        IgCollectionItem ipoint = points.create ();
        ipoint["pos"] = IgV3d(x, y, z);
        association.associate (item, ipoint);		
        
        nGood++;
      } 
      
      else 
      {
        nBad++;
      }
    }

    // Do nStep propagations from track Inner state to track Outer state
    
    GP = gPin - StepVector;
    float w = 0;
    
    for (int istep = 0; istep < nSteps+1 ; ++istep) 
    { // from innerPosition to outerPosition
      GP += StepVector;
      Surface::PositionType pos (GP.x (), GP.y (), GP.z ());
      PlaneBuilder::ReturnType SteppingPlane = PlaneBuilder ().plane (pos, rot);
      TrajectoryStateOnSurface trj_in =  propagator.propagate (FTSin, *SteppingPlane);
      TrajectoryStateOnSurface trj_out =  reversePropagator.propagate (FTSout, *SteppingPlane);
	
      if (trj_in.isValid () && trj_out.isValid ()) 
      {
        float x1 = trj_in.globalPosition ().x () / 100.0;
        float y1 = trj_in.globalPosition ().y () / 100.0;
        float z1 = trj_in.globalPosition ().z () / 100.0;		   

        float x2 = trj_out.globalPosition ().x () / 100.0;
        float y2 = trj_out.globalPosition ().y () / 100.0;
        float z2 = trj_out.globalPosition ().z () / 100.0;		   

        float ww = 0.;
	(w < 0.4999) ? ww = w : ww=1.0-w;
        float w2 = 0.5*ww*ww/((1.0-ww)*(1.0-ww));
        if(w>0.4999) w2=1.0-w2;  

        float x = (1.0-w2)*x1 + w2*x2;
        float y = (1.0-w2)*y1 + w2*y2;
        float z = (1.0-w2)*z1 + w2*z2;

        IgCollectionItem ipoint = points.create ();
        ipoint["pos"] = IgV3d(x,y,z);
        association.associate (item, ipoint);		
    
        nGood++;
      } 
      
      else 
      {
        nBad++;
      }
      
      w += 1.0/float(nSteps);
    }
    
    // Do nStepsInside propagations on Outside to plot track
    GP = gPout;
    
    for (int istep = 0; istep < nStepsOutside; ++istep) 
    {
      GP += StepVector;
      Surface::PositionType pos (GP.x (), GP.y (), GP.z ());
      PlaneBuilder::ReturnType SteppingPlane = PlaneBuilder ().plane (pos, rot);
      TrajectoryStateOnSurface trj =  propagator.propagate (FTSout, *SteppingPlane);
      
      if (trj.isValid ())
      {
        float x = trj.globalPosition ().x () / 100.0;
        float y = trj.globalPosition ().y () / 100.0;
        float z = trj.globalPosition ().z () / 100.0;		   
        IgCollectionItem ipoint = points.create ();
        ipoint["pos"] = IgV3d (x,y,z);
        association.associate (item, ipoint);		
      
        nGood++;
      } 
    
      else 
      {
        nBad++;
      }
    }
  }
}
