#ifndef _Plane_H
#define _Plane_H

#include "math.h"
#include "Object.h"
#include "Color.h"

#include <iostream>
#include <armadillo>

using namespace std;
using namespace arma;

class Plane : public Object {
	mat normal;
	double distance;
	Color color;
	
	public:
	
	Plane ();
	
	Plane (mat, double, Color);
	
	// method functions
	mat getPlaneNormal () { return normal; }
	double getPlaneDistance () { return distance; }
	virtual Color getColor () { return color; }
	
	virtual mat getNormalAt(mat point) {
		return normal;
	}
	
	virtual double findIntersection(Ray ray) {
		mat ray_direction = ray.getRayDirection();
		
		double a = dot(normal, ray_direction);
		
		if (a == 0) {
			// ray is parallel to the plane
			return -1;
		}
		else {
			
			double b = dot(normal,ray.getRayOrigin()+(-normal*distance));
			// cout<<b<<endl;
			return -1*b/a;
		}
	}
	
};

Plane::Plane () {
	normal = mat{1,0,0};
	distance = 0;
	color = Color(0.5,0.5,0.5, 0);
}

Plane::Plane (mat normalValue, double distanceValue, Color colorValue) {
	normal = normalValue;
	distance = distanceValue;
	color = colorValue;
}

#endif
