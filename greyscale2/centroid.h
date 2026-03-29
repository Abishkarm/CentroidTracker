#pragma once

#include <cmath>

#define PI 3.14159265

// Struct for storing centroid data committed to shared memory
struct CentroidData {
	double ic_b, jc_b;
	double ic_g, jc_g;
	double ic_r, jc_r;
	double ic_y, jc_y;
	int frame;
};

// Draw circular pink marker around centroid on an RGB image
void draw_marker(ibyte* p0, int width, int height, int radius, double ic, double jc);
