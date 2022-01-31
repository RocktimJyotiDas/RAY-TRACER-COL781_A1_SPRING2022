#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <limits>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <iostream>
#include <armadillo>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Ray.h"
#include "Camera.h"
#include "Color.h"
#include "Source.h"
#include "Light.h"
#include "Object.h"
#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"
#include "Quadric.h"

using namespace std;
using namespace arma;

struct RGBType {
	double r;
	double g;
	double b;
};

vector<Object*> scene_objects;

void savebmp (const char *filename, int w, int h, int dpi, RGBType *data) {
	FILE *f;
	int k = w*h;
	int s = 4*k;
	int filesize = 54 + s;
	
	double factor = 39.375;
	int m = static_cast<int>(factor);
	
	int ppm = dpi*m;
	
	unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0,0,0, 54,0,0,0};
	unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0,24,0};
	
	bmpfileheader[ 2] = (unsigned char)(filesize);
	bmpfileheader[ 3] = (unsigned char)(filesize>>8);
	bmpfileheader[ 4] = (unsigned char)(filesize>>16);
	bmpfileheader[ 5] = (unsigned char)(filesize>>24);
	
	bmpinfoheader[ 4] = (unsigned char)(w);
	bmpinfoheader[ 5] = (unsigned char)(w>>8);
	bmpinfoheader[ 6] = (unsigned char)(w>>16);
	bmpinfoheader[ 7] = (unsigned char)(w>>24);
	
	bmpinfoheader[ 8] = (unsigned char)(h);
	bmpinfoheader[ 9] = (unsigned char)(h>>8);
	bmpinfoheader[10] = (unsigned char)(h>>16);
	bmpinfoheader[11] = (unsigned char)(h>>24);
	
	bmpinfoheader[21] = (unsigned char)(s);
	bmpinfoheader[22] = (unsigned char)(s>>8);
	bmpinfoheader[23] = (unsigned char)(s>>16);
	bmpinfoheader[24] = (unsigned char)(s>>24);
	
	bmpinfoheader[25] = (unsigned char)(ppm);
	bmpinfoheader[26] = (unsigned char)(ppm>>8);
	bmpinfoheader[27] = (unsigned char)(ppm>>16);
	bmpinfoheader[28] = (unsigned char)(ppm>>24);
	
	bmpinfoheader[29] = (unsigned char)(ppm);
	bmpinfoheader[30] = (unsigned char)(ppm>>8);
	bmpinfoheader[31] = (unsigned char)(ppm>>16);
	bmpinfoheader[32] = (unsigned char)(ppm>>24);
	
	f = fopen(filename,"wb");
	
	fwrite(bmpfileheader,1,14,f);
	fwrite(bmpinfoheader,1,40,f);
	
	for (int i = 0; i < k; i++) {
		RGBType rgb = data[i];
		
		double red = (data[i].r)*255;
		double green = (data[i].g)*255;
		double blue = (data[i].b)*255;
		
		unsigned char color[3] = {(int)floor(blue),(int)floor(green),(int)floor(red)};
		
		fwrite(color,1,3,f);
	}
	
	fclose(f);
}

void Triangle2Cube(mat corner1, mat corner2, Color color){
	double c1x = corner1(0);
	double c1y = corner1(1);
	double c1z = corner1(2);

	double c2x = corner2(0);
	double c2y = corner2(1);
	double c2z = corner2(2);

	mat A{c2x, c1y, c1z};
	mat B{c2x, c1y, c2z};
	mat C{c1x, c1y, c2z};
	
	mat D{c2x, c2y, c1z};
	mat E{c1x, c2y, c1z};
	mat F{c1x, c2y, c2z};

	scene_objects.push_back(new Triangle(D,A,corner1, color));
	scene_objects.push_back(new Triangle(corner1,E,D, color));

	scene_objects.push_back(new Triangle(corner2,B,A, color));
	scene_objects.push_back(new Triangle(A,D,corner2, color));

	scene_objects.push_back(new Triangle(F,C,B, color));
	scene_objects.push_back(new Triangle(B,corner2,F, color));

	scene_objects.push_back(new Triangle(E,corner1,C, color));
	scene_objects.push_back(new Triangle(C,F,E, color));

	scene_objects.push_back(new Triangle(D,E,F, color));
	scene_objects.push_back(new Triangle(F,corner2,D, color));

	scene_objects.push_back(new Triangle(corner1,A,B, color));
	scene_objects.push_back(new Triangle(B,C,corner1, color));
}

int winningObjectIndex(vector<double> object_intersections) {
	// return the index of the winning intersection
	int index_of_minimum_value;
	
	// prevent unnessary calculations
	if (object_intersections.size() == 0) {
		// if there are no intersections
		return -1;
	}
	else if (object_intersections.size() == 1) {
		if (object_intersections.at(0) > 0) {
			// if that intersection is greater than zero then its our index of minimum value
			return 0;
		}
		else {
			// otherwise the only intersection value is negative
			return -1;
		}
	}
	else {
		// otherwise there is more than one intersection
		// first find the maximum value
		
		double max = 0;
		for (int i = 0; i < object_intersections.size(); i++) {
			if (max < object_intersections.at(i)) {
				max = object_intersections.at(i);
			}
		}
		
		// then starting from the maximum value find the minimum positive value
		if (max > 0) {
			// we only want positive intersections
			for (int index = 0; index < object_intersections.size(); index++) {
				if (object_intersections.at(index) > 0 && object_intersections.at(index) <= max) {
					max = object_intersections.at(index);
					index_of_minimum_value = index;
				}
			}
			
			return index_of_minimum_value;
		}
		else {
			// all the intersections were negative
			return -1;
		}
	}
}

Color getColorAt(mat intersection_position, mat intersecting_ray_direction, vector<Object*> scene_objects, int index_of_winning_object, vector<Source*> light_sources, double accuracy, double ambientlight) {
	
	Color winning_object_color = scene_objects.at(index_of_winning_object)->getColor();
	mat winning_object_normal = scene_objects.at(index_of_winning_object)->getNormalAt(intersection_position);
	
	if (winning_object_color.getColorSpecial() == 2) {
		// checkered/tile floor pattern
		
		int square = (int)floor(intersection_position(0)) + (int)floor(intersection_position(2));
		
		if ((square % 2) == 0) {
			// black tile
			winning_object_color.setColorRed(0);
			winning_object_color.setColorGreen(0);
			winning_object_color.setColorBlue(0);
		}
		else {
			// white tile
			winning_object_color.setColorRed(1);
			winning_object_color.setColorGreen(1);
			winning_object_color.setColorRed(1);
		}
	}
	
	Color final_color = winning_object_color.colorScalar(ambientlight);
	
	if (winning_object_color.getColorSpecial() > 0 && winning_object_color.getColorSpecial() <= 1) {
		// reflection from objects with specular intensity
		double dot1 = dot(winning_object_normal,-intersecting_ray_direction);
		mat scalar1 = winning_object_normal*dot1;
		mat add1 = scalar1 + intersecting_ray_direction;
		mat scalar2 = add1*2;
		mat add2 = -intersecting_ray_direction + scalar2;
		mat reflection_direction = normalize(add2);
		
		Ray reflection_ray (intersection_position, reflection_direction);
		
		// determine what the ray intersects with first
		vector<double> reflection_intersections;
		
		for (int reflection_index = 0; reflection_index < scene_objects.size(); reflection_index++) {
			reflection_intersections.push_back(scene_objects.at(reflection_index)->findIntersection(reflection_ray));
		}
		
		int index_of_winning_object_with_reflection = winningObjectIndex(reflection_intersections);
		
		if (index_of_winning_object_with_reflection != -1) {
			// reflection ray missed everthing else
			if (reflection_intersections.at(index_of_winning_object_with_reflection) > accuracy) {
				// determine the position and direction at the point of intersection with the reflection ray
				// the ray only affects the color if it reflected off something
				
				mat reflection_intersection_position = intersection_position + reflection_direction* reflection_intersections.at(index_of_winning_object_with_reflection);

				mat reflection_intersection_ray_direction = reflection_direction;
				
				Color reflection_intersection_color = getColorAt(reflection_intersection_position, reflection_intersection_ray_direction, scene_objects, index_of_winning_object_with_reflection, light_sources, accuracy, ambientlight);
				
				final_color = final_color.colorAdd(reflection_intersection_color.colorScalar(winning_object_color.getColorSpecial()));
			}
		}
	}
	
	for (int light_index = 0; light_index < light_sources.size(); light_index++) {
		mat light_direction = normalize(light_sources.at(light_index)->getLightPosition()  - intersection_position);
		
		float cosine_angle = dot(winning_object_normal,light_direction);
		
		if (cosine_angle > 0) {
			// test for shadows
			bool shadowed = false;
			
			mat distance_to_light = normalize(light_sources.at(light_index)->getLightPosition()- intersection_position);

			float distance_to_light_magnitude = norm(distance_to_light,2);
			
			Ray shadow_ray (intersection_position,
			normalize( light_sources.at(light_index)->getLightPosition()- intersection_position));
			
			vector<double> secondary_intersections;
			
			for (int object_index = 0; object_index < scene_objects.size() && shadowed == false; object_index++) {
				secondary_intersections.push_back(scene_objects.at(object_index)->findIntersection(shadow_ray));
			}
			
			for (int c = 0; c < secondary_intersections.size(); c++) {
				if (secondary_intersections.at(c) > accuracy) {
					if (secondary_intersections.at(c) <= distance_to_light_magnitude) {
						shadowed = true;
					}
					break;
				}
				
			}
			
			if (shadowed == false) {
				final_color = final_color.colorAdd(winning_object_color.colorMultiply(light_sources.at(light_index)->getLightColor()).colorScalar(cosine_angle));
				
				if (winning_object_color.getColorSpecial() > 0 && winning_object_color.getColorSpecial() <= 1) {
					// special [0-1]
					double dot1 = dot(winning_object_normal,-intersecting_ray_direction);

					mat scalar1 = winning_object_normal * dot1;
					mat add1 = scalar1+intersecting_ray_direction;
					mat scalar2 = add1*2;
					mat add2 = -intersecting_ray_direction+ scalar2;
					mat reflection_direction = normalize(add2);
					
					double specular = dot(reflection_direction,light_direction);
					if (specular > 0) {
						specular = pow(specular, 10);
						final_color = final_color.colorAdd(light_sources.at(light_index)->getLightColor().colorScalar(specular*winning_object_color.getColorSpecial()));
					}
				}
				
			}
			
		}
	}
	
	return final_color.clip();
}

int thisone;

int main (int argc, char *argv[]) {
	cout << "rendering ..." << endl;
	
	clock_t t1, t2;
	t1 = clock();
	
	int dpi = 72;
	int width = 640;
	int height = 480;
	int n = width*height;
	RGBType *pixels = new RGBType[n];
	
	int aadepth = 1;
	double aathreshold = 0.1;
	double aspectratio = (double)width/(double)height;
	double ambientlight = 0.2;
	double accuracy = 0.00000001;
	
	mat O {0,0,0};
	mat X {1,0,0};
	mat Y {0,1,0};
	mat Z {0,0,1};
	
	mat new_sphere_location {1.75, -0.25, 0};
	
	mat campos {3, 1.5, -4};
	
	mat look_at {0, 0, 0};
	mat diff_btw = campos - look_at;
	
	mat camdir 	 = normalize(-diff_btw);
	mat camright = normalize(cross(Y, camdir));
	mat camdown  = cross(camright,camdir);

	Camera scene_cam (campos, camdir, camright, camdown);
	
	Color white_light (1.0, 1.0, 1.0, 0);
	Color pretty_green (0.5, 1.0, 0.5, 0.3);
	Color maroon (0.5, 0.25, 0.25, 0);
	Color tile_floor (1, 1, 1, 2);
	Color gray (0.5, 0.5, 0.5, 0);
	Color black (0.0, 0.0, 0.0, 0);
	Color orange (0.94, 0.75, 0.31,0);
	
	mat light_position {-7,10,-10};
	Light scene_light (light_position, white_light);
	mat light_position2 {7,10,10};
	Light scene_light2 (light_position2, white_light);
	vector<Source*> light_sources;

	light_sources.push_back(dynamic_cast<Source*>(&scene_light));
	light_sources.push_back(dynamic_cast<Source*>(&scene_light2));

	// scene objects
	Sphere scene_sphere (O, 1, pretty_green);
	//Sphere scene_sphere2 (new_sphere_location, 0.5, gray);
	Quadric scene_cylinder (new_sphere_location, 0.5, gray, -1,0.5);
	Plane scene_plane (Y, -1, maroon);
	Triangle scene_triangle(mat{-2,-1,0},mat{-3,1,0},mat{-2.5,-1,-1}, orange);

	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_cylinder));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_plane));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_triangle));
	Triangle2Cube(mat{-3,-1,1.5},mat{-3.5,1,2.5}, orange);

	unsigned char image[n*3];
	int thisone, aa_index;
	double xamnt, yamnt;
	double tempRed, tempGreen, tempBlue;
	
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			thisone = y*width + x;
			
			// start with a blank pixel
			double tempRed[aadepth*aadepth];
			double tempGreen[aadepth*aadepth];
			double tempBlue[aadepth*aadepth];
			
			for (int aax = 0; aax < aadepth; aax++) {
				for (int aay = 0; aay < aadepth; aay++) {
			
					aa_index = aay*aadepth + aax;
					
					srand(time(0));
					
					// create the ray from the camera to this pixel
					if (aadepth == 1) {
					
						// start with no anti-aliasing
						if (width > height) {
							// the image is wider than it is tall
							xamnt = ((x+0.5)/width)*aspectratio - (((width-height)/(double)height)/2);
							yamnt = ((height - y) + 0.5)/height;
						}
						else if (height > width) {
							// the imager is taller than it is wide
							xamnt = (x + 0.5)/ width;
							yamnt = (((height - y) + 0.5)/height)/aspectratio - (((height - width)/(double)width)/2);
						}
						else {
							// the image is square
							xamnt = (x + 0.5)/width;
							yamnt = ((height - y) + 0.5)/height;
						}
					}
					else {
						// anti-aliasing
						if (width > height) {
							// the image is wider than it is tall
							xamnt = ((x + (double)aax/((double)aadepth - 1))/width)*aspectratio - (((width-height)/(double)height)/2);
							yamnt = ((height - y) + (double)aax/((double)aadepth - 1))/height;
						}
						else if (height > width) {
							// the imager is taller than it is wide
							xamnt = (x + (double)aax/((double)aadepth - 1))/ width;
							yamnt = (((height - y) + (double)aax/((double)aadepth - 1))/height)/aspectratio - (((height - width)/(double)width)/2);
						}
						else {
							// the image is square
							xamnt = (x + (double)aax/((double)aadepth - 1))/width;
							yamnt = ((height - y) + (double)aax/((double)aadepth - 1))/height;
						}
					}
					
					mat cam_ray_origin = scene_cam.getCameraPosition();
					mat cam_ray_direction = normalize(camdir+ camright*(xamnt-0.5)+camdown*(yamnt - 0.5));
					
					Ray cam_ray (cam_ray_origin, cam_ray_direction);
					
					vector<double> intersections;
					
					for (int index = 0; index < scene_objects.size(); index++) {
						intersections.push_back(scene_objects.at(index)->findIntersection(cam_ray));
					}
					
					int index_of_winning_object = winningObjectIndex(intersections);
					
					if (index_of_winning_object == -1) {
						// set the backgroung black
						tempRed[aa_index] = 0;
						tempGreen[aa_index] = 0;
						tempBlue[aa_index] = 0;
					}
					else{
						// index coresponds to an object in our scene
						if (intersections.at(index_of_winning_object) > accuracy) {
							// determine the position and direction vectors at the point of intersection

							 mat intersection_position = cam_ray_origin+cam_ray_direction*intersections.at(index_of_winning_object);

							mat intersecting_ray_direction = cam_ray_direction;
		
							Color intersection_color = getColorAt(intersection_position, intersecting_ray_direction, scene_objects, index_of_winning_object, light_sources, accuracy, ambientlight);
							
							tempRed[aa_index] = intersection_color.getColorRed();
							tempGreen[aa_index] = intersection_color.getColorGreen();
							tempBlue[aa_index] = intersection_color.getColorBlue();
						}
					}
				}
			}
			
			// average the pixel color
			double totalRed = 0;
			double totalGreen = 0;
			double totalBlue = 0;
			
			for (int iRed = 0; iRed < aadepth*aadepth; iRed++) {
				totalRed = totalRed + tempRed[iRed];
			}
			for (int iGreen = 0; iGreen < aadepth*aadepth; iGreen++) {
				totalGreen = totalGreen + tempGreen[iGreen];
			}
			for (int iBlue = 0; iBlue < aadepth*aadepth; iBlue++) {
				totalBlue = totalBlue + tempBlue[iBlue];
			}
			
			double avgRed = totalRed/(aadepth*aadepth);
			double avgGreen = totalGreen/(aadepth*aadepth);
			double avgBlue = totalBlue/(aadepth*aadepth);
			
			pixels[thisone].r = avgRed;
			pixels[thisone].g = avgGreen;
			pixels[thisone].b = avgBlue;
			
		}
	}


	int arrSize = sizeof(image)/sizeof(image[0]);
	cout << arrSize << endl;

	// 1D buffer array : [ 255 247  ....    ]
	for (int i = 0; i < n; i++){

		unsigned char red 		= (pixels[i].r)*255;
		unsigned char green 	= (pixels[i].g)*255;
		unsigned char blue 		= (pixels[i].b)*255;

		image[i*3+0] = red;
		image[i*3+1] = green;
		image[i*3+2] = blue; 
	}
	cout << "filled" << endl;
	savebmp("scene_anti-aliased.bmp",width,height,dpi,pixels);
	
	delete pixels, tempRed, tempGreen, tempBlue;
	
	// display using glfw window
	GLFWwindow* window;

    if (!glfwInit()) {
        printf("Couldn't init GLFW\n");
        return 1;
    }


    window = glfwCreateWindow(640, 480, "Assignment1", NULL, NULL);
    if (!window) {
        printf("Couldn't open window\n");
        return 1;
    }
	
	glfwMakeContextCurrent(window);

	// Generate texture
    GLuint tex_handle;
    glGenTextures(1, &tex_handle);
    glBindTexture(GL_TEXTURE_2D, tex_handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, image);


    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set up orphographic projection
        int window_width, window_height;
        glfwGetFramebufferSize(window, &window_width, &window_height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, window_width, window_height, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);

        // Render texture
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,tex_handle);

        glBegin(GL_QUADS);
            glTexCoord2d(0,0); glVertex2i(640, 480);
            glTexCoord2d(1,0); glVertex2i(0, 480);
            glTexCoord2d(1,1); glVertex2i(0, 0);
            glTexCoord2d(0,1); glVertex2i(640, 0);
        glEnd();
		
        glDisable(GL_TEXTURE_2D);

        glfwSwapBuffers(window);
        glfwPollEvents();

	}
	t2 = clock();
	float diff = ((float)t2 - (float)t1)/1000;
	
	cout << diff << " seconds" << endl;
	
	return 0;
}
