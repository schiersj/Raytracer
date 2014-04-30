/******************************************************************************
* Program:
*   Raytracer
* Author:
*   Jeffery Schiers
* Desc:
*   Raytracing is a way to render 3D graphics into a 2D space. This Program
*   preforms its calculations using liniar algebra. 
******************************************************************************/
#include <iostream>
#include <vector>
#include <cmath>  // pow() sqrt()
#include <cstdio> // files handling savebmp()
#include <ctime>  // clock()

// header files
#include "vect.h"
//#include "ray.h"
#include "camera.h"
#include "color.h"
#include "sources.h"
#include "objects.h"

using namespace std;

/******************************************************************************
 * RGBTYPE STRUCT - this holds our red green and blue colors
 *****************************************************************************/
struct RGBType
{
   double r; // red
   double g; // green
   double b; // blue
};

/******************************************************************************
 * SAVEBMP - this takes our data and save it as a BMP image file.
 *****************************************************************************/
void savebmp (const char *filename, int w, int h, int dpi, RGBType *data)
{
   FILE *fout;

   int k = w*h;
   int s = 4*k;
   int filesize = 54 + s;

   double factor = 39.375;
   int m = static_cast<int>(factor);

   int ppm = dpi*m;

   unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0,0,0, 54,0,0,0};
   unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0,24,0};

   bmpfileheader[ 2] = (unsigned char)(filesize);
   bmpfileheader[ 3] = (unsigned char)(filesize >> 8);
   bmpfileheader[ 4] = (unsigned char)(filesize >> 16);
   bmpfileheader[ 5] = (unsigned char)(filesize >> 24);

   bmpinfoheader[ 4] = (unsigned char)(w);
   bmpinfoheader[ 5] = (unsigned char)(w >> 8);
   bmpinfoheader[ 6] = (unsigned char)(w >> 16);
   bmpinfoheader[ 7] = (unsigned char)(w >> 24);

   bmpinfoheader[ 8] = (unsigned char)(h);
   bmpinfoheader[ 9] = (unsigned char)(h >> 8);
   bmpinfoheader[10] = (unsigned char)(h >> 16);
   bmpinfoheader[11] = (unsigned char)(h >> 24);

   bmpinfoheader[21] = (unsigned char)(s);
   bmpinfoheader[22] = (unsigned char)(s >> 8);
   bmpinfoheader[23] = (unsigned char)(s >> 16);
   bmpinfoheader[24] = (unsigned char)(s >> 24);

   bmpinfoheader[25] = (unsigned char)(ppm);
   bmpinfoheader[26] = (unsigned char)(ppm >> 8);
   bmpinfoheader[27] = (unsigned char)(ppm >> 16);
   bmpinfoheader[28] = (unsigned char)(ppm >> 24);

   bmpinfoheader[29] = (unsigned char)(ppm);
   bmpinfoheader[30] = (unsigned char)(ppm >> 8);
   bmpinfoheader[31] = (unsigned char)(ppm >> 16);
   bmpinfoheader[32] = (unsigned char)(ppm >> 24);

   fout = fopen(filename, "wb");

   fwrite(bmpfileheader,1,14,fout);
   fwrite(bmpinfoheader,1,40,fout);

   for (int i = 0; i < k; i++)
   {
      double red   = (data[i].r)*255;
      double green = (data[i].g)*255;
      double blue  = (data[i].b)*255;

      unsigned char color[3] = {(int)floor(blue),
                                (int)floor(green), 
                                (int)floor(red)};

      fwrite(color,1,3,fout);
   }

   fclose(fout);
}

/******************************************************************************
 * WINNING OBJECT INDEX - returns the index of the winning intersection
 *****************************************************************************/
int winningObjectIndex(vector<double> objInt)
{
   int iMinVal; // index of minimum value
   int size = objInt.size();

   if (size == 0)        // no objects
      return -1;
   else if (size == 1)   // only one object
   {
      if (objInt.at(0) > 0)
         return 0; // this is it
      else
         return -1; // it missed
   }
   else                  // many objects
   {
      // first find the max value
      double max = 0;
      for (int i = 0; i < size; i++)
         if (max < objInt.at(i))
            max = objInt.at(i);

      // then find the min positive value
      if (max > 0)
      {
         for (int i = 0; i < size; i++)
            if (objInt.at(i) > 0 && objInt.at(i) <= max)
            {
               max = objInt.at(i);
               iMinVal = i;
            }
         return iMinVal;
      }
      else // all intersections were negative
         return -1;
   }
}

/******************************************************************************
 * GET COLOR AT - returns the color determained by ray intersections
 *****************************************************************************/
Color getColorAt(Vect intPos, Vect intDir, vector<Object*> sObjects
                , int iWinObj, vector<Source*> lSources
                , double accuracy, double ambientlight)
{
   Color iWinColor = sObjects.at(iWinObj)->getColor();
   Vect iWinNorm = sObjects.at(iWinObj)->getNormalAt(intPos);

   // this adds the checkerboard
   if (iWinColor.getColorSpecial() == 2)
   {
      // checkered/tile floor pattern
      int square = (int)floor(intPos.getVectX()) + (int)floor(intPos.getVectZ());

      if ((square % 2) == 0)         // black tile
         iWinColor = Color(0,0,0,0);
      else                           // white tile
         iWinColor = Color(1,1,1,0);
   }

   Color finalColor = iWinColor.colorScalar(ambientlight);

   // reflection
   if (iWinColor.getColorSpecial() > 0 && iWinColor.getColorSpecial() <= 1)
   {
      // reflection from objects with specular intensity
      double dot1  = iWinNorm.dotProduct(intDir.negative());
      Vect scalar1 = iWinNorm.vectMult(dot1);
      Vect add1    = scalar1.vectAdd(intDir);
      Vect scalar2 = add1.vectMult(2);
      Vect add2    = intDir.negative().vectAdd(scalar2);
      Vect refDir  = add2.normalize();

      Ray reflectRay (intPos, refDir);

      // determine what the ray intersects with first
      vector<double> reflectInt;

      for (int iRef = 0; iRef < sObjects.size(); iRef++)
      {
         reflectInt.push_back(sObjects.at(iRef)->findIntersection(reflectRay));
      }

      int iWinObjReflect = winningObjectIndex(reflectInt);

      if (iWinObjReflect != -1)
      {
         // reflection ray missed everything else
         if(reflectInt.at(iWinObjReflect) > accuracy)
         {
            // determin the position and direction at the point of intersection
            // the ray only affects the color if it reflected off something
            Vect refIntPos = intPos.vectAdd(refDir.vectMult(reflectInt.at(iWinObjReflect)));
            Vect refIntDir = refDir;

            // this process is recursive
            Color refIntColor = getColorAt(refIntPos, refIntDir, sObjects, 
                                           iWinObjReflect, lSources, accuracy, ambientlight);

            finalColor = finalColor.colorAdd(refIntColor.colorScalar(iWinColor.getColorSpecial()));
         }
      }
   }

   // shadows
   for (int iLight = 0; iLight < lSources.size(); iLight++)
   {
      Vect lightDir = lSources.at(iLight)->getLightPosition().vectAdd(intPos.negative()).normalize();

      float cosAngle = iWinNorm.dotProduct(lightDir);

      if (cosAngle > 0)
      {
         // test for shadows
         bool shadowed = false;

         Vect lightDist = lSources.at(iLight)->getLightPosition().vectAdd(intPos.negative()).normalize();
         float lightDistMagnitude = lightDist.magnitude();

         Ray shadowRay (intPos, lSources.at(iLight)->getLightPosition().vectAdd(intPos.negative()).normalize());
         
         vector<double> intersectTwo;

         for (int iObj = 0; iObj < sObjects.size() && shadowed == false; iObj++)
            intersectTwo.push_back(sObjects.at(iObj)->findIntersection(shadowRay));

         for (int c = 0; c < intersectTwo.size(); c++)
         {
            if (intersectTwo.at(c) > accuracy)
            {
               if (intersectTwo.at(c) <= lightDistMagnitude)
               {
                  shadowed = true;
               }
               break;
            }
         }

         if (shadowed == false)
         {
            finalColor = finalColor.colorAdd(iWinColor.colorMultiply(lSources.at(iLight)->getLightColor()).colorScalar(cosAngle));

            // shinines
            if (iWinColor.getColorSpecial() > 0 && iWinColor.getColorSpecial() <= 1)
            {
               // special [0-1]
               double dot1 = iWinNorm.dotProduct(intDir.negative());
               Vect scalar1 = iWinNorm.vectMult(dot1);
               Vect add1 = scalar1.vectAdd(intDir);
               Vect scalar2 = add1.vectMult(2);
               Vect add2 = intDir.negative().vectAdd(scalar2);
               Vect refDir = add2.normalize();

               double specular = refDir.dotProduct(lightDir);
               if (specular > 0)
               {
                  specular = pow(specular, 10);
                  finalColor = finalColor.colorAdd(lSources.at(iLight)->getLightColor().colorScalar(specular*iWinColor.getColorSpecial()));
               }
            }
         }
      }
   }

   return finalColor.clip();
}

/******************************************************************************
 * MAIN
 *****************************************************************************/
int main (int argc, char *argv[])
{
   cout << ">>> RENDERING..." << endl;

   // time the process
   clock_t start, end;
   start = clock();

   // initialize variables
   int dpi    = 72;
   int width  = 640;
   int height = 480;
   int n = width * height; // total pixels in image
   RGBType *pixels = new RGBType[n];

   // scene properties
   int aadepth = 1; // the higher the value the longer it takes
   double accuracy = 0.00000001;
   double ambientlight = 0.2;
   double aathreshold = 0.1;
   double aspectratio = (double)width / (double)height;
   
   // standard vectors
   Vect X (1,0,0);
   Vect Y (0,1,0);
   Vect Z (0,0,1);
   Vect O (0,0,0); // origin

   Vect Pos1 ( 1.75, -0.25, 0);
   Vect Pos2 (-1.75, -0.25, 0);

   // define camera
   Vect campos (3, 1.5, -4);
   Vect focus = O;
   Vect diffBTW ( campos.getVectX() - focus.getVectX()
                , campos.getVectY() - focus.getVectY()
                , campos.getVectZ() - focus.getVectZ() );

   Vect camdir = diffBTW.negative().normalize();
   Vect camright = Y.crossProduct(camdir).normalize();
   Vect camdown = camright.crossProduct(camdir);
   Camera camera (campos, camdir, camright, camdown);

   // colors
   Color white  ( 1.0,  1.0,  1.0, 0); // special 0 = solid color
   Color gray   ( 0.5,  0.5,  0.5, 0);
   Color black  ( 0.0,  0.0,  0.0, 0);
   Color maroon ( 0.5, 0.25, 0.25, 0);
   Color orange (0.94, 0.75, 0.31, 0);

   Color greenShine  ( 0.5,  1.0,  0.5, 0.3); // special [0,1] reflective
   Color maroonShine ( 0.5, 0.25, 0.25, 0.3);
   Color orangeShine (0.94, 0.75, 0.31, 0.3);
   Color blackShine  (0,0,0,0.3);

   Color tile (1, 1, 1, 2); // special 2 = checkered

   // scene objects
   Sphere sphere1 (   O,    1,  greenShine);
   Sphere sphere2 (Pos1, 0.75, maroonShine);
   Sphere sphere3 (Pos2, 0.75, orangeShine);
   Plane ground (Y, -1, tile);
   //Triangle scene_triangle (Vect(3,0,0), Vect(0,3,0), Vect(0,0,3), orange);

   vector<Object*> sObjects;
   sObjects.push_back(dynamic_cast<Object*>(&sphere1));
   sObjects.push_back(dynamic_cast<Object*>(&sphere2));
   sObjects.push_back(dynamic_cast<Object*>(&sphere3));
   sObjects.push_back(dynamic_cast<Object*>(&ground));
   //sObjects.push_back(dynamic_cast<Object*>(&scene_triangle));

   //makeCube(Vect (1,1,1), Vect (-1,-1,-1), orange);

   // light source (s)
   Vect lightPos1 (-7,10,-10);
   //Vect lightPos2 (14,10,-10);
   Light light1 (lightPos1, white);
   //Light light2 (lightPos2, gray);

   vector<Source*> lSources;
   lSources.push_back(dynamic_cast<Source*>(&light1));
   //lSources.push_back(dynamic_cast<Source*>(&light2));

   int curPixel, aaIndex;
   double xamnt, yamnt; // amounts
   //double tempRed, tempGreen, tempBlue;

   // go through every pickel and return the color;
   for (int x = 0; x < width; x++)
   {
      for (int y = 0; y < height; y++)
      {
         curPixel = y * width + x; // the actual pixel coordinates

         // start with a blank pixel
         double tempRed[aadepth*aadepth];
         double tempGreen[aadepth*aadepth];
         double tempBlue[aadepth*aadepth];

         for (int aax = 0; aax < aadepth; aax++)
         {
            for (int aay = 0; aay < aadepth; aay++)
            {
               aaIndex = aay*aadepth + aax;

               //srand(time(0));

               // create the ray from the camera to this pixel
               if (aadepth == 1)
               {
                  // start with no anti-aliasing
                  if (width > height)
                  {
                     // the image is wider than it is tall
                     xamnt = ((x+0.5)/width)*aspectratio - (((width - height)/(double)height)/2);
                     yamnt = ((height -y)+0.5)/height;
                  }
                  else if (height > width)
                  {
                     // the image is taller than it is wide
                     xamnt = (x + 0.5)/width;
                     yamnt = (((height-y)+0.5)/height)/aspectratio - (((height - width)/(double)width)/2);
                  }
                  else
                  {
                     // the image is square
                     xamnt = (x + 0.5)/width;
                     yamnt = ((height - y)+ 0.5)/height;
                  }
               }
               else
               {
                  // anti-aliasing
                  if (width > height)
                  {
                     // the image is wider than it is tall
                     xamnt = ((x+(double)aax/((double)aadepth - 1))/width)*aspectratio 
                           - (((width - height)/(double)height)/2);
                     yamnt = ((height -y)+(double)aax/((double)aadepth - 1))/height;
                  }
                  else if (height > width)
                  {
                     // the image is taller than it is wide
                     xamnt = (x + (double)aax/((double)aadepth - 1))/width;
                     yamnt = (((height-y)+(double)aax/((double)aadepth - 1))/height)/aspectratio 
                           - (((height - width)/(double)width)/2);
                  }
                  else
                  {
                     // the image is square
                     xamnt = (x + (double)aax/((double)aadepth - 1))/width;
                     yamnt = ((height - y)+ (double)aax/((double)aadepth - 1))/height;
                  }
               }


               // create rays
               Vect camRayOrg = camera.getCameraPosition();
               Vect camRayDir = camdir.vectAdd(camright.vectMult(xamnt - 0.5)
                                              .vectAdd(camdown.vectMult(yamnt - 0.5))).normalize();

               Ray cam_ray (camRayOrg, camRayDir);

               vector<double> intersections;

               for (int index = 0; index < sObjects.size(); index++)
               {
                  intersections.push_back(sObjects.at(index)->findIntersection(cam_ray));
               }

               int iWinObj = winningObjectIndex(intersections);

               // return color
               if(iWinObj == -1)
               {
                  // set the background black
                  tempRed[aaIndex] = 0;
                  tempGreen[aaIndex] = 0;
                  tempBlue[aaIndex] = 0;
               }
               else
               {
                  // index coresponds to an object in our scene
                  if (intersections.at(iWinObj) > accuracy)
                  {
                     // determin the position and direction vectors at the point of intersection

                     Vect intPos = camRayOrg.vectAdd(camRayDir.vectMult(intersections.at(iWinObj)));
                     Vect intDir = camRayDir;

                     Color intersectColor = getColorAt( intPos, intDir, sObjects
                                                      , iWinObj, lSources, accuracy, ambientlight);

                     tempRed[aaIndex] = intersectColor.getColorRed();
                     tempGreen[aaIndex] = intersectColor.getColorGreen();
                     tempBlue[aaIndex] = intersectColor.getColorBlue();
                  }
               }
            }
         }

         // average the pixel color
         double totalRed = 0;
         double totalGreen = 0;
         double totalBlue = 0;

         for (int iRed = 0; iRed < aadepth*aadepth; iRed++)
            totalRed = totalRed + tempRed[iRed];
         for (int iGreen = 0; iGreen < aadepth*aadepth; iGreen++)
            totalGreen = totalGreen + tempGreen[iGreen];
         for (int iBlue = 0; iBlue < aadepth*aadepth; iBlue++)
            totalBlue = totalBlue + tempBlue[iBlue];

         double avgRed = totalRed/(aadepth*aadepth);
         double avgGreen = totalGreen/(aadepth*aadepth);
         double avgBlue = totalBlue/(aadepth*aadepth);

         pixels[curPixel].r = avgRed;
         pixels[curPixel].g = avgGreen;
         pixels[curPixel].b = avgBlue;
      }
   }

   // save our pixels to the image
   savebmp("scene.bmp", width, height, dpi, pixels);

   // clean up
   delete pixels;//, tempRed, tempGreen, tempBlue;

   // end the time and display the render time
   end = clock();
   float diff = ((float)end - (float)start)/1000;
   cout << diff << " seconds" << endl;

   return 0;
}
