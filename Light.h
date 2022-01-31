#ifndef _Light_H
#define _Light_H

#include "Source.h"
#include "Color.h"

#include <armadillo>
using namespace arma;

class Light : public Source {
	mat position;
	Color color;
	
	public:
	
	Light ();
	
	Light (mat, Color);
	
	// method functions
	virtual mat getLightPosition () { return position; }
	virtual Color getLightColor () { return color; }
	
};

Light::Light () {
	position = mat{0,0,0};
	color = Color(1,1,1, 0);
}

Light::Light (mat p, Color c) {
	position = p;
	color = c;
}

#endif
