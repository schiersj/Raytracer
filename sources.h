/******************************************************************************
* Header:
*   Sources
* Desc:
*   This file contains the Source Base class and all the Subclasses:
*   (Light)
******************************************************************************/
#ifndef SOURCES_H
#define SOURCES_H

/******************************************************************************
 * SOURCE CLASS - base class for our sources
 *****************************************************************************/
class Source
{
public:
   virtual Vect getLightPosition() { return Vect (0, 0, 0);  }
   virtual Color getLightColor()   { return Color (1,1,1,0); }
};

/******************************************************************************
 * LIGHT CLASS - A light source for our scene
 *****************************************************************************/
class Light : public Source
{
private:
   Vect position;
   Color color;

public:
   Light() // default constructor
   {
      position = Vect(0,0,0);
      color = Color(1,1,1,0);
   }

   Light(Vect p, Color c) // secondary constructor
   {
      position = p;
      color    = c;
   }

   virtual Vect getLightPosition() { return position; }
   virtual Color getLightColor()   { return color;    }
};

#endif
