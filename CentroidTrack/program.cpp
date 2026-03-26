#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cmath>
#include <Windows.h>

#define KEY(c) ( GetAsyncKeyState((int)(c)) & (SHORT)0x8000 )

using namespace std;

#include "timer.h"
#include "image_transfer.h"
#include "vision.h"
#include "centroid.h"


int main()
{
	//console output
	AllocConsole();
	freopen("CONOUT$", "w", stdout);

	int radius, width, height, size, cam_number, pthresh;
	double ic_b, jc_b, ic_g, jc_g, ic_r, jc_r, ic_y, jc_y, signal;
	image rgb1, rgb2, rgb3;
	ibyte* p0;

	//SHARED MEMORY SETUP
	HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(CentroidData), "CentroidSharedMem"); //Creating mapping object CentroidSharedMem
	if (hMapFile == NULL) { cout << "Failed to create shared memory\n"; return 1; } 
	//Pointer of struct type dereferenced to shared memory containing centroid positions 
	CentroidData* sharedData = (CentroidData*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(CentroidData)); 
	if (sharedData == NULL) { CloseHandle(hMapFile); cout << "Failed to map shared memory\n"; return 1; }
	sharedData->frame = 0;

	activate_vision();

	cout << "\npress space key to acquire image\n";
	while (!KEY(VK_SPACE));

	cam_number = 0; //Adjust this for your camera
	width = 640;
	height = 480;
	radius = 3; //Centroid marker radius (pixels)

	activate_camera(cam_number, height, width);

	rgb1.width = width;  rgb1.height = height;  rgb1.type = RGB_IMAGE;
	rgb2.width = width;  rgb2.height = height;  rgb2.type = RGB_IMAGE;
	rgb3.width = width;  rgb3.height = height;  rgb3.type = RGB_IMAGE;

	allocate_image(rgb1);
	allocate_image(rgb2);
	allocate_image(rgb3);

	size = width * height;

	//pixel filtering (user adj.) 
	//sets the number of pixels for the vertical neighbouring pixel search in the centroid search function
	pthresh = 1;

	bool bHeld = false, gHeld = false, rHeld = false, yHeld = false;

	while (1) {

		double t_start = high_resolution_time(); //loop timer

		//blue threshold user adjustment
		if (KEY('B') && !bHeld) {
			bHeld = true;
			char channel; int val;
			cout << "Adjust blue filter - enter channel (b/g/r): ";
			cin >> channel;
			cout << "Enter value (0-255): ";
			cin >> val;
			if (val >= 0 && val <= 255) {
				if      (channel == 'b') blueFilter.thresh = val;
				else if (channel == 'g') blueFilter.ch1    = val;
				else if (channel == 'r') blueFilter.ch2    = val;
				else cout << "Invalid channel, rejected\n";
			}
			else cout << "Out of range, rejected\n";
		}
		if (!KEY('B')) bHeld = false;

		//green threshold user adjustment
		if (KEY('G') && !gHeld) {
			gHeld = true;
			char channel; int val;
			cout << "Adjust green filter - enter channel (b/g/r): ";
			cin >> channel;
			cout << "Enter value (0-255): ";
			cin >> val;
			if (val >= 0 && val <= 255) {
				if      (channel == 'g') greenFilter.thresh = val;
				else if (channel == 'b') greenFilter.ch1    = val;
				else if (channel == 'r') greenFilter.ch2    = val;
				else cout << "Invalid channel, rejected\n";
			}
			else cout << "Out of range, rejected\n";
		}
		if (!KEY('G')) gHeld = false;

		//red threshold user adjustment
		if (KEY('R') && !rHeld) {
			rHeld = true;
			char channel; int val;
			cout << "Adjust red filter - enter channel (r/g/b): ";
			cin >> channel;
			cout << "Enter value (0-255): ";
			cin >> val;
			if (val >= 0 && val <= 255) {
				if      (channel == 'r') redFilter.thresh = val;
				else if (channel == 'g') redFilter.ch1    = val;
				else if (channel == 'b') redFilter.ch2    = val;
				else cout << "Invalid channel, rejected\n";
			}
			else cout << "Out of range, rejected\n";
		}
		if (!KEY('R')) rHeld = false;

		//yellow threshold user adjustment
		if (KEY('Y') && !yHeld) {
			yHeld = true;
			char channel; int val;
			cout << "Adjust yellow filter - enter channel (y/b): ";
			cin >> channel;
			cout << "Enter value (0-255): ";
			cin >> val;
			if (val >= 0 && val <= 255) {
				if      (channel == 'y') yellowFilter.thresh = val;
				else if (channel == 'b') yellowFilter.ch1    = val;
				else cout << "Invalid channel, rejected\n";
			}
			else cout << "Out of range, rejected\n";
		}
		if (!KEY('Y')) yHeld = false;

		//pixel user adjustment
		if (KEY('P')) {
			int newPthresh;
			cout << "Enter pixel threshold (0-255): ";
			cin >> newPthresh;
			if (newPthresh >= 0 && newPthresh <= 255)
				pthresh = newPthresh;
			else
				cout << "Out of range, rejected\n";
		}

		acquire_image(rgb1, cam_number);
		scale(rgb1, rgb2); //scaling image output
		copy(rgb2, rgb3); //copy to rgb3 for inspection 
		p0 = rgb2.pdata; //cast pointer to rgb2

		//Calculates centroids based on user adj. colour thresholds and vertical neighbouring pixel check
		calc_centroid(p0, width, height, blueFilter,   pthresh, ic_b, jc_b);
		calc_centroid(p0, width, height, greenFilter,  pthresh, ic_g, jc_g);
		calc_centroid(p0, width, height, redFilter,    pthresh, ic_r, jc_r);
		calc_centroid(p0, width, height, yellowFilter, pthresh, ic_y, jc_y);

		//Draws pink marker around centroid
		draw_marker(p0, width, height, radius, ic_b, jc_b);
		draw_marker(p0, width, height, radius, ic_g, jc_g);
		draw_marker(p0, width, height, radius, ic_r, jc_r);
		draw_marker(p0, width, height, radius, ic_y, jc_y);

		//commit centroids to shared memory
		sharedData->ic_b = ic_b; sharedData->jc_b = jc_b;
		sharedData->ic_g = ic_g; sharedData->jc_g = jc_g;
		sharedData->ic_r = ic_r; sharedData->jc_r = jc_r;
		sharedData->ic_y = ic_y; sharedData->jc_y = jc_y;
		sharedData->frame++;

		double loop_time = high_resolution_time() - t_start; //timer

		view_rgb_image(rgb2);

		if (KEY(VK_RETURN))
			cout << "\n--- centroids ---"
			     << "\n  blue:   ic=" << ic_b << " jc=" << jc_b
			     << "\n  green:  ic=" << ic_g << " jc=" << jc_g
			     << "\n  red:    ic=" << ic_r << " jc=" << jc_r
			     << "\n  yellow: ic=" << ic_y << " jc=" << jc_y
			     << "\n loop time=" << loop_time
			     << "\n--- blue filter ---"
			     << "\n  b(thresh)=" << blueFilter.thresh << "  g(ch1)=" << blueFilter.ch1 << "  r(ch2)=" << blueFilter.ch2
			     << "\n--- green filter ---"
			     << "\n  g(thresh)=" << greenFilter.thresh << "  b(ch1)=" << greenFilter.ch1 << "  r(ch2)=" << greenFilter.ch2
			     << "\n--- red filter ---"
			     << "\n  r(thresh)=" << redFilter.thresh << "  g(ch1)=" << redFilter.ch1 << "  b(ch2)=" << redFilter.ch2
			     << "\n--- yellow filter ---"
			     << "\n  y(thresh)=" << yellowFilter.thresh << "  b(ch1)=" << yellowFilter.ch1
			     << "\n--- pixel threshold ---"
			     << "\n  pthresh=" << pthresh << "\n";

		if (KEY('X')) break;
	}

	save_rgb_image("rgb1.bmp", rgb1);
	save_rgb_image("rgb3.bmp", rgb3);
	free_image(rgb2);
	deactivate_vision();

	//release shared memory
	UnmapViewOfFile(sharedData);
	CloseHandle(hMapFile);

	cout << "\n\ndone.\n";
	return 0;
}
