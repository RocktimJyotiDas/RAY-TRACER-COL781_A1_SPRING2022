#ifndef _RAY_H
#define _RAY_H

#include "math.h"
#include <armadillo>

using namespace arma;

class Ray{
    mat origin, direction;
    double x,y,z;

    public:
    Ray();
    Ray(mat, mat);
    mat getRayOrigin(){return origin;}
    mat getRayDirection(){return direction;}
    
};
Ray::Ray(){
    origin = mat{0,0,0};
    direction = mat{1,0,0};
}

Ray::Ray(mat o, mat d){
    origin = o;
    direction = d;
}

#endif