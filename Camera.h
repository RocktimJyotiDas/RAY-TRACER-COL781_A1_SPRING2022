#ifndef _Camera_H
#define _Camera_H

#include <armadillo>

using namespace arma;

class Camera {
	mat campos, camdir, camright, camdown;
	
	public:
	
	Camera ();
	
	Camera (mat, mat, mat, mat);
	
	// method functions
	mat getCameraPosition () { return campos; }
	mat getCameraDirection () { return camdir; }
	mat getCameraRight () { return camright; }
	mat getCameraDown () { return camdown; }
	
};

Camera::Camera () {
	campos = mat{0,0,0};
	camdir = mat{0,0,1};
	camright = mat{0,0,0};
	camdown = mat{0,0,0};
}

Camera::Camera (mat pos, mat dir, mat right, mat down) {
	campos = pos;
	camdir = dir;
	camright = right;
	camdown = down;
}

#endif
