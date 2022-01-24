#ifndef _RAY_H
#define _RAY_H

#include "Vect.h"
#include "math.h"
class Ray{
    Vect origin, direction;
    double x,y,z;

    public:
    Ray();
    Ray(Vect, Vect);
    Vect getRayOrigin(){return origin;}
    Vect getRayDirection(){return direction;}
    
};
Ray::Ray(){
    origin = Vect(0,0,0);
    direction = Vect(1,0,0);
}

Ray::Ray(Vect o, Vect d){
    origin = o;
    direction = d;
}

#endif