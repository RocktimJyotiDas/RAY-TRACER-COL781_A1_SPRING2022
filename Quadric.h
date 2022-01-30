#ifndef _QUADRIC_H
#define _QUADRIC_H

#include "math.h"
#include "Object.h"
#include "Vect.h"
#include "Color.h"

class Quadric : public Object {
	Vect center;
	double radius;
	Color color;
	double height_buttom;
	double height_top;
	
	public:
	
	Quadric ();
	
	Quadric (Vect, double, Color, double, double);
	
	// method functions
	Vect getQuadricCenter () { return center; }
	double getQuadricRadius () { return radius; }
	double getQuadricHeight_Buttom() {return height_buttom;}
	double getQuadricHeight_Top() {return height_top;}
	virtual Color getColor () { return color; }
	
	virtual Vect getNormalAt(Vect point) {
		double h_ =  point.getVectY();
		Vect center_h  = center.vectAdd(Vect(0,h_,0));
		Vect normal_Vect = point.vectAdd(center_h.negative()).normalize();
		return normal_Vect;
	}
	
	virtual double findIntersection(Ray ray) {
		Vect ray_origin = ray.getRayOrigin();
		double ray_origin_x = ray_origin.getVectX();
		double ray_origin_y = ray_origin.getVectY();
		double ray_origin_z = ray_origin.getVectZ();
		
		Vect ray_direction = ray.getRayDirection();
		double ray_direction_x = ray_direction.getVectX();
		double ray_direction_y = ray_direction.getVectY();
		double ray_direction_z = ray_direction.getVectZ();
		
		Vect sphere_center = center;
		double sphere_center_x = sphere_center.getVectX();
		double sphere_center_y = sphere_center.getVectY();
		double sphere_center_z = sphere_center.getVectZ();
		
		double a = pow(ray_direction_x, 2) + pow(ray_direction_z, 2); // normalized
		double b = (2*(ray_origin_x - sphere_center_x)*ray_direction_x) + (2*(ray_origin_z - sphere_center_z)*ray_direction_z);
		double c = pow(ray_origin_x - sphere_center_x, 2) + pow(ray_origin_z - sphere_center_z, 2) - (radius*radius);
		
		double discriminant = b*b - 4*a*c;
		
		if (discriminant > 0) {
			/// the ray intersects the sphere
			
			// the first root
			double root_1 = ((-1*b - sqrt(discriminant))/(2*a)) - 0.000001;
			double root_2 = ((sqrt(discriminant) - b)/2) - 0.000001;
			double y_intersection = ray_origin_y + root_1*ray_direction_y;
			double y_intersection2 = ray_origin_y + root_2*ray_direction_y;
			if (root_1 > 0) {
				if(y_intersection > height_top || y_intersection < height_buttom){
					if(y_intersection2 > height_top || y_intersection < height_buttom){
						return -1;
					}
					else{
						return root_2;
					}	
				}
				// the first root is the smallest positive root
				return root_1;
			}
			else {
				// the second root is the smallest positive root
				if(y_intersection2 > height_top || y_intersection < height_buttom){
					return -1;
				}
				else{
					return root_2;
				}
				
			}
		}
		else {
			// the ray missed the sphere
			return -1;
		}
	}
	
};

Quadric::Quadric () {
	center = Vect(0,0,0);
	radius = 1.0;
	color = Color(0.5,0.5,0.5, 0);
	height_buttom = -1;
	height_top = 1;
}

Quadric::Quadric (Vect centerValue, double radiusValue, Color colorValue, double height1, double height2) {
	center = centerValue;
	radius = radiusValue;
	color = colorValue;
	height_buttom = height1;
	height_top = height2;
}

#endif
