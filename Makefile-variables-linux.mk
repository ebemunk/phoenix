CC = g++
CFLAGS = -g -std=c++0x
BUILD_DIR = build
OCV_LIBS = -lopencv_core -lopencv_highgui -lopencv_imgproc
BOOST_LIBS = -lboost_program_options -lboost_filesystem -lboost_system
LDFLAGS = $(OCV_LIBS) $(BOOST_LIBS)
EXE_NAME = phoenix