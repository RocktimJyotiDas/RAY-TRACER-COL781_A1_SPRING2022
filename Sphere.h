#ifndef _SPHERE_H
#define _SPHERE_H

#include "math.h"
#include "Object.h"
#include "Color.h"
#include <armadillo>

using namespace arma;

class Sphere : public Object {
	mat center;
	double radius;
	Color color;
	
	public:
	
	Sphere ();
	
	Sphere (mat, double, Color);
	
	// method functions
	mat getSphereCenter () { return center; }
	double getSphereRadius () { return radius; }
	virtual Color getColor () { return color; }
	
	virtual mat getNormalAt(mat point) {
		// normal always points away from the center of a sphere
		mat normal_Vect = normalize(point -center);
		return normal_Vect;
	}
	
	virtual double findIntersection(Ray ray) {
		mat ray_origin = ray.getRayOrigin();
		double ray_origin_x = ray_origin(0);
		double ray_origin_y = ray_origin(1);
		double ray_origin_z = ray_origin(2);
		
		mat ray_direction = ray.getRayDirection();
		double ray_direction_x = ray_direction(0);
		double ray_direction_y = ray_direction(1);
		double ray_direction_z = ray_direction(2);
		
		mat sphere_center = center;
		double sphere_center_x = sphere_center(0);
		double sphere_center_y = sphere_center(1);
		double sphere_center_z = sphere_center(2);
		
		double a = 1; // normalized
		double b = (2*(ray_origin_x - sphere_center_x)*ray_direction_x) + (2*(ray_origin_y - sphere_center_y)*ray_direction_y) + (2*(ray_origin_z - sphere_center_z)*ray_direction_z);
		double c = pow(ray_origin_x - sphere_center_x, 2) + pow(ray_origin_y - sphere_center_y, 2) + pow(ray_origin_z - sphere_center_z, 2) - (radius*radius);
		
		double discriminant = b*b - 4*c;
		
		if (discriminant > 0) {
			/// the ray intersects the sphere
			
			// the first root
			double root_1 = ((-1*b - sqrt(discriminant))/2) - 0.000001;
			
			if (root_1 > 0) {
				// the first root is the smallest positive root
				return root_1;
			}
			else {
				// the second root is the smallest positive root
				double root_2 = ((sqrt(discriminant) - b)/2) - 0.000001;
				return root_2;
			}
		}
		else {
			// the ray missed the sphere
			return -1;
		}
	}
	
};

Sphere::Sphere () {
	center = mat{0,0,0};
	radius = 1.0;
	color = Color(0.5,0.5,0.5, 0);
}

Sphere::Sphere (mat centerValue, double radiusValue, Color colorValue) {
	center = centerValue;
	radius = radiusValue;
	color = colorValue;
}

#endif
