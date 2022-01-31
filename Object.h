#ifndef _OBJECT_H
#define _OBJECT_H

#include "Ray.h"
#include "Color.h"

#include <armadillo>

using namespace arma;

// Normalise vector with second norm
mat normalize(mat M){
    return M/norm(M,2);
}

class Object {	
	public:
	
	Object ();
	
	// method functions
	virtual Color getColor () { return Color (0.0, 0.0, 0.0, 0); }
	
	virtual mat getNormalAt(mat intersection_position) {
		return mat{0, 0, 0};
	}
	
	virtual double findIntersection(Ray ray) {
		return 0;
	}
	
};

Object::Object () {}

#endif
