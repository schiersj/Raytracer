/******************************************************************************
* Header:
*   Objects
* Desc:
*   This file contains the Object Base class and all the Subclasses:
*   (Plain, Sphere, Triangle)
******************************************************************************/
#ifndef OBJECTS_H
#define OBJECTS_H

/******************************************************************************
 * OBJECT CLASS - This the base class for all objects
 *****************************************************************************/
class Object 
{
public:
   virtual Color getColor ()                 { return Color(0,0,0,0); }
   virtual Vect getNormalAt(Vect pos)        { return Vect (0,0,0);   }
   virtual double findIntersection (Ray ray) { return 0;              }
};

/******************************************************************************
 * PLAIN CLASS - This contains everything needed to create a plane
 *****************************************************************************/
class Plane : public Object
{
private:
   Vect normal;
   double distance;
   Color color;

public:
   Plane() : normal(Vect(1,0,0)), distance(0), color(Color(0.5,0.5,0.5,0)) {}
   Plane(Vect n, double d, Color c) : normal(n), distance(d), color(c)     {}

   Vect getPlaneNormal()     { return normal;   }
   double getPlaneDistance() { return distance; }

   // virtual functions
   virtual Color getColor()                 { return color;  }
   virtual Vect getNormalAt(Vect point)     { return normal; }
   virtual double findIntersection(Ray ray)
   {
      Vect rayDir = ray.getRayDirection();
      double a = rayDir.dotProduct(normal);

      if (a == 0) // the ray is parallel to the plane
         return -1;
      else
      {
         double b = normal.dotProduct(ray.getRayOrigin().vectAdd(
                    normal.vectMult(distance).negative()));
         return -1*b/a;
      }
   }
};

/******************************************************************************
 * SPHERE CLASS - this contains everything needed to create a sphere object
 *****************************************************************************/
class Sphere : public Object
{
private:
   Vect center;
   double radius;
   Color color;

public:
   Sphere() // default constructor
   {
      center = Vect(0,0,0);
      radius = 1.0;
      color  = Color(0.5,0.5,0.5,0);
   }

   Sphere(Vect iCenter, double iRadius, Color iColor) // secondary constructor
   {
      center = iCenter;
      radius = iRadius;
      color  = iColor;
   }

   Vect getSphereCenter ()   { return center; }
   double getSphereRadius () { return radius; }

   // virtual functions
   virtual Color getColor () { return color;  }

   virtual Vect getNormalAt(Vect point)
   {
      // normal always points away from the center of a sphere
      return point.vectAdd(center.negative()).normalize();
   }

   virtual double findIntersection(Ray ray)
   {
      // get the ray origin coordinates
      Vect rayOrg = ray.getRayOrigin();
      double rayOrgx = rayOrg.getVectX();
      double rayOrgy = rayOrg.getVectY();
      double rayOrgz = rayOrg.getVectZ();

      // get the ray direction coordinates
      Vect rayDir = ray.getRayDirection();
      double rayDirx = rayDir.getVectX();
      double rayDiry = rayDir.getVectY();
      double rayDirz = rayDir.getVectZ();

      // get the sphere center corrdinates
      double centerx = center.getVectX();
      double centery = center.getVectY();
      double centerz = center.getVectZ();

      // calculate the discriminant
      double b = (2 * (rayOrgx - centerx) * rayDirx) 
               + (2 * (rayOrgy - centery) * rayDiry) 
               + (2 * (rayOrgz - centerz) * rayDirz);
      double c = pow(rayOrgx - centerx, 2) 
               + pow(rayOrgy - centery, 2) 
               + pow(rayOrgz - centerz, 2)
               - (radius*radius);

      double discriminant = b*b - 4*c;

      if (discriminant > 0) // the ray intersects with the sphere
      {
         double root1 = ((-1*b - sqrt(discriminant)) / 2) - 0.000001;

         if (root1 > 0)   // first root is the smallest
            return root1;
         else             // second root is the smallest
            return ((sqrt(discriminant) - b)/2) - 0.000001;
      }
      else // it missed
         return -1;
   }
};

/******************************************************************************
 * TRIANGLE CLASS - creates a sing triangle plane. can be used to make squares
 *****************************************************************************/
class Triangle : public Object
{
private:
   Vect A, B, C, normal;
   double distance;
   Color color;

public:
   Triangle() // default constructor
   {
      A = Vect(1,0,0);
      B = Vect(0,1,0);
      C = Vect(0,0,1);
      color = Color(0.5,0.5,0.5,0);
   }

   Triangle(Vect iA, Vect iB, Vect iC, Color iColor) // secondary constructor
   {
      A = iA;
      B = iB;
      C = iC;
      color = iColor;
   }

   Vect getTriangleNormal()
   {
      Vect CA ( C.getVectX() - A.getVectX()
              , C.getVectY() - A.getVectY()
              , C.getVectZ() - A.getVectZ());
      Vect BA ( B.getVectX() - A.getVectX()
              , B.getVectY() - A.getVectY()
              , B.getVectZ() - A.getVectZ());

      return CA.crossProduct(BA).normalize();
   }

   double getTriangleDistance()
   {
      normal = getTriangleNormal();
      return normal.dotProduct(A);
   }

   // virtual functions
   virtual Color getColor()             { return color;               }
   virtual Vect getNormalAt(Vect point) { return getTriangleNormal(); }

   virtual double findIntersection(Ray ray)
   {
      Vect rayDir = ray.getRayDirection();
      Vect rayOrg = ray.getRayOrigin();

      normal = getTriangleNormal();
      distance = getTriangleDistance();

      double a = rayDir.dotProduct(normal);

      if (a == 0) // ray is parallel to the triangle
         return -1;
      else
      {
         double b = normal.dotProduct(ray.getRayOrigin().vectAdd(
                    normal.vectMult(distance).negative()));
         double distToPlane = -1*b/a;

         // point of intersection
         double Qx = rayDir.vectMult(distToPlane).getVectX() + rayOrg.getVectX();
         double Qy = rayDir.vectMult(distToPlane).getVectY() + rayOrg.getVectY();
         double Qz = rayDir.vectMult(distToPlane).getVectZ() + rayOrg.getVectZ();

         Vect Q (Qx, Qy, Qz);

         // [CAxQA]*a >= 0
         Vect CA ( C.getVectX() - A.getVectX()
                 , C.getVectY() - A.getVectY()
                 , C.getVectZ() - A.getVectZ());
         Vect QA ( Q.getVectX() - A.getVectX()
                 , Q.getVectY() - A.getVectY()
                 , Q.getVectZ() - A.getVectZ());
         double test1 = (CA.crossProduct(QA)).dotProduct(normal);

         // [BCxQC]*a >= 0
         Vect BC ( B.getVectX() - C.getVectX()
                 , B.getVectY() - C.getVectY()
                 , B.getVectZ() - C.getVectZ());
         Vect QC ( Q.getVectX() - C.getVectX()
                 , Q.getVectY() - C.getVectY()
                 , Q.getVectZ() - C.getVectZ());
         double test2 = (BC.crossProduct(QC)).dotProduct(normal);

         // [ABxQB]*a >= 0
         Vect AB ( A.getVectX() - B.getVectX()
                 , A.getVectY() - B.getVectY()
                 , A.getVectZ() - B.getVectZ());
         Vect QB ( Q.getVectX() - B.getVectX()
                 , Q.getVectY() - B.getVectY()
                 , Q.getVectZ() - B.getVectZ());
         double test3 = (AB.crossProduct(QB)).dotProduct(normal);

         if((test1 >= 0) && (test2 >= 0) && (test3 >= 0)) // inside the triangle
            return distToPlane;
         else // outside the triangle
            return -1;
      }
   }
};

#endif
