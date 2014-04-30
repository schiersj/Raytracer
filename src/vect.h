/******************************************************************************
* Header:
*   Vect
* Desc:
*   This file contains the Vect class.
******************************************************************************/
#ifndef VECT_H
#define VECT_H

/******************************************************************************
 * VECT CLASS - 
 *****************************************************************************/
class Vect
{
private:
   double x, y, z; // cordinates in the xyz space

public:
   Vect() : x(0), y(0), z(0) {}                             // default const
   Vect(double i, double j, double k) : x(i), y(j), z(k) {} // secondary const

   double getVectX()   { return x; }
   double getVectY()   { return y; }
   double getVectZ()   { return z; }

   double magnitude()      { return sqrt((x*x) + (y*y) + (z*z)); }

   double dotProduct(Vect v) 
   {
      return x*v.getVectX() + y*v.getVectY() + z*v.getVectZ();
   }

   Vect crossProduct (Vect v)
   {
      return Vect ( y*v.getVectZ() - z*v.getVectY()
                  , z*v.getVectX() - x*v.getVectZ()
                  , x*v.getVectY() - y*v.getVectX() );
   }

   Vect negative()         { return Vect( -x,  -y,  -z); }
   Vect vectMult(double s) { return Vect(x*s, y*s, z*s); } // s=scalar
   Vect vectAdd (Vect v)
   {
      return Vect (x + v.getVectX(), y + v.getVectY(), z + v.getVectZ());
   }

   Vect normalize()
   {
      double maginitude = sqrt((x*x) + (y*y) + (z*z));
      return Vect ( x / maginitude
                  , y / maginitude
                  , z / maginitude );
   }
};

/******************************************************************************
 * RAY CLASS - The rays trace the pixels to the respecive intersections
 *****************************************************************************/
class Ray
{
private:
   Vect origin, direction;

public:
   Ray() : origin(Vect(0,0,0)), direction(Vect(1,0,0)) {} // default const
   Ray(Vect o, Vect d) : origin(o), direction(d) {}       // secondary const
   Vect getRayOrigin()    { return origin;    }
   Vect getRayDirection() { return direction; }
};

#endif
