phoenix - Image Forensics
=======
phoenix is a small image forensics tool that can run some common analyses on images. It was inspired by the image analyses at the [Hackerfactor Blog](http://www.hackerfactor.com/blog/), and implements some of the algorithms used there. It is named phoenix because it has resurrected many times before taking its final form in C++.

Features:

* Error Level Analysis
* Luminance Gradient
* Average Distance
* HSV and Lab colorspace histograms
* ~~JPEG quality estimate ([ImageMagick](http://www.imagemagick.org/script/index.php)-style and [Hackerfactor jpegquality](http://www.hackerfactor.com/src/jpegquality.c) estimates)~~

#Usage
* `-h | -help` display help text.
* `-f | -file <path>` Required, the path to the source image.
* `-o | -output [path]` Save results in files (as PNG), optionally to `path`.
* `-d | -display` Display results.
* `-ela` Error Level Analysis
* `-lg` Luminance Gradient
* `-avgdist` Average Distance
* `-hsv` HSV Colorspace Histogram
* -`lab` Lab Colorspace Histogram

#Screenshots
Here are some examples of phoenix output with the image used in the legendary [Body By Victoria](http://www.hackerfactor.com/blog/?/archives/322-Body-By-Victoria.html) analysis by Neal Krawetz.

##ELA (Error Level Analysis)
![Error Level Analysis](assets/bbv_ela.png)

##LG (Luminance Gradient)
![Luminance Gradient](assets/bbv_lg.png)

##AVGDIST (Average-Distance of Neighbor Pixels)
![Average Distance](assets/bbv_avgdist.png)

##HSV Colorspace Histogram
![HSV Histogram](assets/bbv_hsv.png)

##Lab Colorspace Histogram
![Lab Histogram](assets/bbv_lab.png)

#Compiling
phoenix depends on OpenCV 2.4.6 and Boost 1.54.0 Libraries. Exact versions are probably not required. Edit the variables in `Makefile-variables.mk` to match your OpenCV and Boost library paths. Running `make` should compile if everything is in place.

#Resources
Some resources I used while developing many of the algorithms here.

* [Hackerfactor Blog](http://www.hackerfactor.com/blog/)
* [Quality Time with Your JPEGs](http://blog.apokalyptik.com/2009/09/16/quality-time-with-your-jpegs/)
* [Exploring JPEG](https://www.imperialviolet.org/binary/jpeg/)
* [ELA From Scratch](https://infohost.nmt.edu/~schlake/ela/)
* [elsamuko Image Forensics](https://sites.google.com/site/elsamuko/forensics)