#!/bin/bash
#provision a clean ubuntu installation

sudo su

#update list first
apt-get update

#install boost dependencies
apt-get install -y build-essential

#fetch boost 1.55.0 and compile
wget http://sourceforge.net/projects/boost/files/boost/1.55.0/boost_1_55_0.tar.bz2/download -O boost_1_55_0.tar.bz2
tar --bzip2 -xf boost_1_55_0.tar.bz2
cd boost_1_55_0
./bootstrap.sh --with-libraries=system,filesystem,program_options
./b2 install

#install opencv dependencies
apt-get install -y git cmake libgtk2.0 libgtk2.0-dev pkg-config python-dev python-numpy libjpeg-dev libpng-dev libtiff-dev libjasper-dev unzip

#fetch opencv 2.4.9 and compile
cd ..
wget http://sourceforge.net/projects/opencvlibrary/files/opencv-unix/2.4.9/opencv-2.4.9.zip/download -O opencv-2.4.9.zip
unzip opencv-2.4.9.zip
cd opencv-2.4.9
mkdir release
cd release
cmake -D CMAKE_BUILD_TYPE=RELEASE \
-D CMAKE_INSTALL_PREFIX=/usr/local \
-D BUILD_opencv_flann=OFF \
-D BUILD_opencv_features2d=OFF \
-D BUILD_opencv_calib3d=OFF \
-D BUILD_opencv_ml=OFF \
-D BUILD_opencv_video=OFF \
-D BUILD_opencv_objdetect=OFF \
-D BUILD_opencv_contrib=OFF \
-D BUILD_opencv_nonfree=OFF \
-D BUILD_opencv_gpu=OFF \
-D BUILD_opencv_legacy=OFF \
-D BUILD_opencv_photo=OFF \
-D BUILD_opencv_python=OFF \
-D BUILD_opencv_stitching=OFF \
-D BUILD_opencv_ts=OFF \
-D BUILD_opencv_videostab=OFF \
-D BUILD_opencv_apps=OFF \
-D BUILD_TESTS=OFF \
-D BUILD_PERF_TESTS=OFF \
..
make -j2
make install

#update libs
ldconfig

#good to compile from this point
cd ../..
git clone https://github.com/ebemunk/phoenix.git
cd phoenix
#comment out the WIN variable in makefile
sed -i '38 s/^/#/' Makefile
make