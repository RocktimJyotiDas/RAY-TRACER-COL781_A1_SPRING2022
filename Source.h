#ifndef _SOURCE_H
#define _SOURCE_H

#include <armadillo>

using namespace arma;

#include "Color.h"

class Source {
	public:
	
	Source();
	
	virtual mat getLightPosition() {return mat{0, 0, 0};}
	virtual Color getLightColor() {return Color(1, 1, 1, 0);}
	
};

Source::Source() {}

#endif
