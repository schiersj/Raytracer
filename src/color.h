/******************************************************************************
* Header:
*   Color
* Desc:
*   Contains the Color class and its funcitons. Used to store and manipulate
*   the colors of every pixel.
******************************************************************************/
#ifndef COLOR_H
#define COLOR_H

/******************************************************************************
 * COLOR CLASS - everything you need for color
 *****************************************************************************/
class Color
{
private:
   double red, green, blue, special;

public:
   Color() : red(0.5), green(0.5), blue(0.5), special(0) {} // default const
   Color(double r, double g, double b, double s) 
                 : red(r), green(g), blue(b), special(s) {} // secondary const

   double getColorRed()     { return red;     }
   double getColorGreen()   { return green;   }
   double getColorBlue()    { return blue;    }
   double getColorSpecial() { return special; }

   double setColorRed(double redValue)         { red = redValue;         }
   double setColorGreen(double greenValue)     { green = greenValue;     }
   double setColorBlue(double blueValue)       { blue = blueValue;       }
   double setColorSpecial(double specialValue) { special = specialValue; }

   double brightness() { return (red + green + blue) / 3; }
   Color colorScalar(double s) { return Color (red*s, green*s, blue*s, special); }

   Color colorAdd(Color color)
   {
      return Color ( red   + color.getColorRed()
                   , green + color.getColorGreen()
                   , blue  + color.getColorBlue()
                   , special);
   }

   Color colorMultiply(Color color)
   {
      return Color ( red   * color.getColorRed()
                   , green * color.getColorGreen()
                   , blue  * color.getColorBlue()
                   , special);
   }

   Color colorAverage(Color color)
   {
      return Color ( (red   + color.getColorRed()) / 2
                   , (green + color.getColorGreen()) / 2
                   , (blue  + color.getColorBlue()) / 2
                   , special);
   }

   Color clip()
   {
      double allLight = red + green + blue;
      double excessLight = allLight - 3;
      if (excessLight > 0)
      {
         red += excessLight * (red / allLight);
         green += excessLight * (green / allLight);
         blue += excessLight  * (blue / allLight);
      }
      if (red > 1)   { red = 1;   }
      if (green > 1) { green = 1; }
      if (blue > 1)  { blue = 1;  }
      if (red < 0)   { red = 0;   }
      if (green < 0) { green = 0; }
      if (blue < 0)  { blue = 0;  }

      return Color (red, green, blue, special);
   }
};

#endif
