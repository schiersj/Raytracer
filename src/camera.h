/******************************************************************************
* Header:
*   Camera
* Desc:
*   Contains the Camera class. The camera is the viewpoint into the scene.
******************************************************************************/
#ifndef CAMERA_H
#define CAMERA_H

/******************************************************************************
 * CAMERA CLASS - everything needed for the camera
 *****************************************************************************/
class Camera
{
private:
   Vect camPos, camDir, camRight, camDown;

public:
   Camera() : camPos(Vect(0,0,0)), camDir(Vect(0,0,1))
            , camRight(Vect(0,0,0)), camDown(Vect(0,0,0))    {} // default const

   Camera (Vect pos, Vect dir, Vect right, Vect down)
      : camPos(pos), camDir(dir), camRight(right), camDown(down) {} // secondary const

   Vect getCameraPosition()  { return camPos;   }
   Vect getCameraDirection() { return camDir;   }
   Vect getCamRight()        { return camRight; }
   Vect getCamDown()         { return camDown;  }
};

#endif
