Centroid Track Program

Searches for red,blue,green,yellow centroids and stores coordination in shared memory. 
Thresholds should be adjusted to account for lighting,camera, and shade of objects

STEPS:

1.Run the code and ensure your camera number is appropriately set

2.You will be prompted in the console to press space to start capturing images

3.Open Imageview.exe to view image output (recommend pinning to task bar)

4.At any time you may press 'enter' in the console to see the following info:

-centroid positions
-colour thresholds for each filter 
-time for last image processing loop to complete

5.The colour thresholds can be adjusted in the console by pressing 'r','g','b','y' for red,green,blue,yellow respectively
6.You will then be prompted to choose again for r,g,b or primary/secondary for yellow

7.The threshold for the neighbouring pixel check can also be adjusted by entering 'p' into the console (current default is 1 pixel)

8.Press 'x' in the console to terminate session. The last frames are saved under rgb1,rgb2,rgb3 in the solution's directory

*Most of the loops seem to execute within 0.05s meaning at max velocity of 0.5 m/s we are losing 2.5cm of accuracy. 
Given the dimensions of the sandwich box being ~15cm in width/length, this seems to be acceptable for now  

IMAGE PROCESSING:
This current version does not do any greyscale processing of the image for reduced complexity 

The current search algorithm filters pixels for desired colours
The threshold will need to be adjusted depending on lighting and shade of objects used. More improvements to come there 
There is a check for neighbouring pixels up/down of the same colour to avoid including stray pixels in the centroid calculation


SHARED DATA: 

The centroid data is stored with the CentroidData struct and then comitted to shared memory object CentroidSharedMem
It can be accessed with FILE_MAP_READ as per https://learn.microsoft.com/en-us/windows/win32/memory/creating-named-shared-memory

