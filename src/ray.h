/******************************************************************************
* Header:
*   Ray
* Desc:
*   This file contains the Ray class.
******************************************************************************/
#ifndef RAY_H
#define RAY_H

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
