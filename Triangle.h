#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#include "math.h"
#include "Object.h"
#include "Color.h"
#include <armadillo>

using namespace arma;


class Triangle : public Object {
    mat A, B, C;
	mat normal;
	double distance;
	Color color;
	
	public:
	
	Triangle ();
	
	Triangle (mat, mat, mat, Color);
	
	// method functions
	mat getTriangleNormal () { 
        mat CA = C-A;
        mat BA = B-A;
        normal = normalize(cross(CA,BA));
        return normal; }

	double getTriangleDistance () {
        normal = getTriangleNormal();
        distance = dot(normal,A);
         return distance; }

	virtual Color getColor () { return color; }
	
	virtual mat getNormalAt(mat point) {
        normal = getTriangleNormal();
		return normal;
	}
	
	virtual double findIntersection(Ray ray) {
		mat ray_direction = ray.getRayDirection();
		mat ray_origin = ray.getRayOrigin();

        normal = getTriangleNormal();
        distance = getTriangleDistance();
		double a = dot(ray_direction,normal);
		
		if (a == 0) {
			// ray is parallel to the Triangle
			return -1;
		}
		else {
			double b = dot(normal, ray.getRayOrigin() - normal*distance);
			double distance2plane = -1*b/a;
            double Qx = ray_direction(0)*distance2plane + ray_origin(0);
            double Qy = ray_direction(1)*distance2plane + ray_origin(1);
            double Qz = ray_direction(2)*distance2plane + ray_origin(2);
            
            mat Q = {Qx, Qy, Qz};
            mat CA = C-A;
            mat QA = Q-A;
            double test1 = dot(cross(CA,QA),normal);

            mat BC = B-C;
            mat QC = Q-C;
            double test2 = dot(cross(BC,QC),normal);

            mat AB = A-B;
            mat QB = Q-B;
            double test3 = dot(cross(AB,QB),normal);

            if ((test1>=0)&(test2>=0)&(test3>=0)){
                return distance2plane;
            }
            else{
                return -1;
            }
		}
	}
	
};

Triangle::Triangle () {
	mat A = {1,0,0};
    mat B = {0,1,0};
    mat C = {0,0,1};
	color = Color(0.5,0.5,0.5, 0);
}

Triangle::Triangle (mat A_coord, mat B_coord, mat C_coord, Color colorValue) {
	A = A_coord;
    B = B_coord;
    C = C_coord;
	color = colorValue;
}

#endif
