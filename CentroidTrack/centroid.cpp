#include <cmath>
#include "image_transfer.h"
#include "centroid.h"

ColorFilter blueFilter   = { 0, 100, 150, 100,  true, 255,   0,   0 };
ColorFilter greenFilter  = { 1,  90,  90,  80,  true,   0, 255,   0 };
ColorFilter redFilter    = { 2, 200,  80,  80,  true,   0,   0, 255 };
ColorFilter yellowFilter = { 2, 200, 150,   0, false,   0, 255, 255 };

void calc_centroid(ibyte* p0, int width, int height, ColorFilter& f, int pthresh, double& ic, double& jc)
{
	int i, j, k, q, up[6][3], down[6][3], product_up, product_down;
	double m, mi, mj;
	ibyte* p, * p_U, * p_D;
	double eps = 1.0e-10;

	int ca = (f.primary + 1) % 3;
	int cb = (f.primary + 2) % 3;

	m = 0; mi = 0; mj = 0;

	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {

			k = i + 640 * j;
			p = p0 + 3 * k;

			product_up = 1;
			product_down = 1;

			for (q = 1; q <= pthresh; q++) {
				if (j > pthresh && j < (height - pthresh)) {
					p_U = p + 3 * 640 * q;
					p_D = p - 3 * 640 * q;
				}
				else if (j <= pthresh) {
					p_U = p + 3 * 640 * q;
					p_D = p;
				}
				else {
					p_U = p;
					p_D = p - 3 * 640 * q;
				}

				if (f.singleChannel) {
					up[q][0] = *(p_U + f.primary) > f.thresh;
					up[q][1] = *(p_U + ca) < f.ch1;
					up[q][2] = *(p_U + cb) < f.ch2;

					down[q][0] = *(p_D + f.primary) > f.thresh;
					down[q][1] = *(p_D + ca) < f.ch1;
					down[q][2] = *(p_D + cb) < f.ch2;
				}
				else {
					up[q][0] = *(p_U + 2) > f.thresh;
					up[q][1] = *(p_U + 1) > f.thresh;
					up[q][2] = *(p_U)     < f.ch1;

					down[q][0] = *(p_D + 2) > f.thresh;
					down[q][1] = *(p_D + 1) > f.thresh;
					down[q][2] = *(p_D)     < f.ch1;
				}

				product_up   *= up[q][0]   * up[q][1]   * up[q][2];
				product_down *= down[q][0] * down[q][1] * down[q][2];
			}

			int B = *p, G = *(p + 1), R = *(p + 2);
			int match = 0;

			if (f.singleChannel)
				match = (*(p + f.primary) > f.thresh) && (*(p + ca) < f.ch1) && (*(p + cb) < f.ch2);
			else
				match = (R > f.thresh) && (G > f.thresh) && (B < f.ch1);

			if (match && (product_up == 1 || product_down == 1)) {
				*p = f.hB; *(p + 1) = f.hG; *(p + 2) = f.hR;
				m  += 255;
				mi += i * 255;
				mj += j * 255;
			}
		}
	}

	ic = mi / (m + eps);
	jc = mj / (m + eps);
}

void draw_marker(ibyte* p0, int width, int height, int radius, double ic, double jc)
{
	int r, theta, i1, j1;
	ibyte* pm;
	int kc = (int)ic + width * (int)jc;

	if (kc < 0 || kc >= width * height) return;

	for (r = 0; r <= radius; r++) {
		for (theta = 0; theta < 360; theta++) {
			i1 = (int)(ic + r * cos(theta * PI / 180.0));
			j1 = (int)(jc + r * sin(theta * PI / 180.0));
			if (i1 >= 0 && i1 < width && j1 >= 0 && j1 < height) {
				pm = p0 + 3 * (i1 + 640 * j1);
				*pm = 180; *(pm + 1) = 105; *(pm + 2) = 255;
			}
		}
	}
}
