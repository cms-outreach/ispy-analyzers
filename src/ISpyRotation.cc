#include "ISpy/Analyzers/interface/ISpyRotation.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"


void ISpyRotation::getAxisAngle(const GeomDet* det, Basic3DVector<double>& axis, double& angle)
{
  TkRotation<double> r = det->surface().rotation();
 
  Basic3DVector<double> nx(1.0, 0.0, 0.0); 
  Basic3DVector<double> ny(0.0, 1.0, 0.0); 
  Basic3DVector<double> nz(0.0, 0.0, 1.0); 
  
  Basic3DVector<double> rx = r*nx; 
  Basic3DVector<double> ry = r*ny; 
  Basic3DVector<double> rz = r*nz; 
   
  double t = rx.x()+ry.y()+rz.z();

  double d = sqrt((ry.z()-rz.y())*(ry.z()-rz.y()) +
		  (rz.x()-rx.z())*(rz.x()-rx.z()) +
		  (rx.y()-ry.x())*(rx.y()-ry.x()));
     
  double cosa = (t-1)*0.5;
  double sina = 0.5*d;
     
  angle = atan2(sina,cosa);
         
  axis = Basic3DVector<double>((rz.y()-ry.z()) / 2*sina,
			       (rx.z()-rz.x()) / 2*sina,
			       (ry.x()-rx.y()) / 2*sina);

  axis = axis.unit();
}
