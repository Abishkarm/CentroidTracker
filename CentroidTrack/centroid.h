#pragma once

#include <cmath>

#define PI 3.14159265

// color filter: thresh = primary channel lower bound, ch1/ch2 = secondary channel upper bounds
// primary = BGR index of dominant channel (0=B, 1=G, 2=R)
// singleChannel = true for single-channel colours (blue, green, red); false for multi-channel (e.g. yellow)
// hB,hG,hR = highlight color painted on matched pixels
struct ColorFilter {
	int primary;
	int thresh;
	int ch1, ch2;
	bool singleChannel;
	ibyte hB, hG, hR;
};

// struct for storing centroid data. To be committed to shared memory
struct CentroidData {
	double ic_b, jc_b;
	double ic_g, jc_g;
	double ic_r, jc_r;
	double ic_y, jc_y;
	int frame;
};

// Initial Filters (user adj.)
extern ColorFilter blueFilter;
extern ColorFilter greenFilter;
extern ColorFilter redFilter;
extern ColorFilter yellowFilter;

// Calculates centroid location, colours all captured pixels to full intensity
// Uses a vertical up/down check for neighbouring pixels of the same colour to manage stray pixels
void calc_centroid(ibyte* p0, int width, int height, ColorFilter& f, int pthresh, double& ic, double& jc);

// Draw circular pink marker around centroid
void draw_marker(ibyte* p0, int width, int height, int radius, double ic, double jc);
