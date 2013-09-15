CC = g++
CFLAGS = -Wall -g -std=c++0x

OCV_INC = C:\opencv_2_4_6\build\include
BOOST_INC = C:\boost_1_54_0

INC_PATHS = -I$(OCV_INC) -I$(BOOST_INC)

OCV_LIBS = -lopencv_highgui246 -lopencv_imgproc246 -lopencv_core246
BOOST_LIBS = -lboost_program_options-mgw48-mt-1_54 -lboost_filesystem-mgw48-mt-1_54 -lboost_system-mgw48-mt-1_54

OCV_LIB_STATIC_ROOT = C:\opencv_2_4_6\mybuild
OCV_LIB_DYN_ROOT = C:\opencv_2_4_6\mybuild\bin
BOOST_LIB_ROOT = C:\boost_1_54_0\stage\lib

DYNAMIC_LINK =

ifdef DYNAMIC_LINK
LDFLAGS = -L$(OCV_LIB_DYN_ROOT) \
		$(OCV_LIBS) \
		-L$(BOOST_LIB_ROOT)\program_options\build\gcc-mingw-4.8.1\release \
		-L$(BOOST_LIB_ROOT)\filesystem\build\gcc-mingw-4.8.1\release \
		-L$(BOOST_LIB_ROOT)\system\build\gcc-mingw-4.8.1\release \
		$(BOOST_LIBS)
EXE_NAME = phoenix-dyn.exe
else
LDFLAGS = -LC:\opencv_2_4_6\mybuild\3rdparty\lib \
		-L$(OCV_LIB_STATIC_ROOT)\lib \
		$(OCV_LIBS) \
		-lzlib -llibjpeg -llibtiff -llibpng -lcomctl32 -lgdi32 \
		-LC:\boost_1_54_0\stage\lib \
		$(BOOST_LIBS) \
		-static
EXE_NAME = phoenix.exe
endif