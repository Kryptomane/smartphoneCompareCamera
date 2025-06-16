# Smartphone Compare Tool

This tools can be used to compare a Lightvalue (L) for different focal lengths of smartphones. The database in background is currently a json file. 
This tools compares the PHYSICALLY values of the Cameras in Smartphones, based on sensorsize, lens, OIS, etc. 

If a smartphone does not have the required focal length, this is implemented using digital zoom (crop). We assume a target resolution of 12 megapixels; 
if, for example, the digital zoom is more than a factor of 2 and the sensor actually installed on the cell phone has a 50 megapixel sensor, an in-sensor zoom is used up to a zoom factor of 2. 
Higher zoom levels reduce the resolution and, of course, the image quality considerably.

# Install

You will need QT 6.7 or later and an uptodate compiler. Check it out, run cmake, build Application

# License

It is free to use, if you adapt it, please let me now. 
It is not allowed to delete my name as author. 
Developed by Silas : speedysilas@outlook.de 
