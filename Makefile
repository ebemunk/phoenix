CC = g++
CFLAGS = -Wall -g -std=c++0x

OCV_INC = C:\Users\Ebemunk\Downloads\opencv\build\include
BOOST_INC = C:\Users\ebemunk\Downloads\boost_1_54_0

INC_PATHS = -I$(OCV_INC) -I$(BOOST_INC)

OCV_LIBS = -lopencv_highgui245 -lopencv_imgproc245 -lopencv_core245
BOOST_LIBS = -lboost_program_options-mgw47-1_54 -lboost_filesystem-mgw47-1_54 -lboost_system-mgw47-1_54

OCV_LIB_STATIC_ROOT = C:\Users\Ebemunk\Desktop\opencv-static
OCV_LIB_DYN_ROOT = C:\Users\ebemunk\Downloads\opencv\release\lib
BOOST_LIB_ROOT = C:\boost-build\boost\bin.v2\libs

DYNAMIC_LINK =

ifdef DYNAMIC_LINK
LDFLAGS = -L$(OCV_LIB_DYN_ROOT) \
		$(OCV_LIBS) \
		-L$(BOOST_LIB_ROOT)\program_options\build\gcc-mingw-4.7.2\release \
		-L$(BOOST_LIB_ROOT)\filesystem\build\gcc-mingw-4.7.2\release \
		-L$(BOOST_LIB_ROOT)\system\build\gcc-mingw-4.7.2\release \
		$(BOOST_LIBS)
EXE_NAME = phoenix-dyn.exe
else
LDFLAGS = -L$(OCV_LIB_STATIC_ROOT)\3rdparty\lib \
		-L$(OCV_LIB_STATIC_ROOT)\lib \
		$(OCV_LIBS) \
		-lzlib -lIlmImf -llibjpeg -llibtiff -llibpng -llibjasper -lcomctl32 -lgdi32 \
		-L$(BOOST_LIB_ROOT)\program_options\build\gcc-mingw-4.7.2\release\link-static \
		-L$(BOOST_LIB_ROOT)\filesystem\build\gcc-mingw-4.7.2\release\link-static \
		-L$(BOOST_LIB_ROOT)\system\build\gcc-mingw-4.7.2\release\link-static \
		$(BOOST_LIBS) \
		-static
EXE_NAME = phoenix.exe
endif

all: phoenix.o functions.o
	$(CC) phoenix.o functions.o $(CFLAGS) $(LDFLAGS) -o $(EXE_NAME)

phoenix.o: phoenix.cpp functions.o
	$(CC) -c phoenix.cpp $(INC_PATHS)

functions.o: functions.cpp
	$(CC) -c functions.cpp -I$(OCV_INC)

.PHONY: clean
clean:
	rm *.o phoenix.exe phoenix-dyn.exe