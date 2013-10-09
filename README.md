phoenix - Image Forensics
=======
phoenix is a small image forensics tool that can run some common analyses on images. Features:

* Error Level Analysis
* Luminance Gradient
* Average Distance
* HSV and Lab colorspace histograms
* JPEG quality estimate

#Usage
Help text can be accessed by `phoenix -help` or `phoenix -h`.

* `-h | -help` display help text.
* `-f | -file <path>` Required, the path to the source image.
* `-o | -output [path]` Save results in files (as PNG), optionally to `path` folder.
* `-d | -display` Display results.
* `-ela` Error Level Analysis
* `-lg` Luminance Gradient
* `-avgdist` Average Distance
* `-hsv` HSV Colorspace Histogram
* -`lab` Lab Colorspace Histogram
* `-borders` Show RGB colorspace borders in the histogram analyses

#Compiling
phoenix depends on OpenCV 2.4.6 and Boost 1.54.0 Libraries. Exact versions are probably not required. Edit the variables in `Makefile-variables.mk` to match your OpenCV and Boost library paths. Running `make` should compile if everything is in place.