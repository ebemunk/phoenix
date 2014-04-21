#
# COMPILER CONFIG
#
#compiler details
CXX = g++
CXXFLAGS = -g -std=c++0x -O3

#project structure
OBJ_DIR = build/obj
BIN_DIR = build
EXE_NAME = phoenix

#source files and corresponding objects
SOURCES = debugger.cpp functions.cpp phoenix.cpp
OBJECTS = $(SOURCES:%.cpp=$(OBJ_DIR)/%.o)

#included header file locations
OCV_INC = C:\opencv_2_4_6\build\include
BOOST_INC = C:\boost_1_54_0
INC_PATHS = -isystem$(OCV_INC) -isystem$(BOOST_INC)

#
# LINKER CONFIG
#
#used libraries
OCV_LIBS = -lopencv_highgui246 -lopencv_imgproc246 -lopencv_core246
BOOST_LIBS = -lboost_program_options-mgw48-mt-1_54 -lboost_filesystem-mgw48-mt-1_54 -lboost_system-mgw48-mt-1_54
WIN_LIBS = -lzlib -llibjpeg -llibtiff -llibpng -lcomctl32 -lgdi32

#location of STATIC builds
OCV_LIB_ROOT = C:\opencv_2_4_6\mybuild
BOOST_LIB_ROOT = C:\boost_1_54_0\stage\lib

#linker options
LDLIBS = $(OCV_LIBS) $(BOOST_LIBS)
LDPATHS = -L$(OCV_LIB_ROOT)/3rdparty/lib -L$(OCV_LIB_ROOT)/lib -L$(BOOST_LIB_ROOT)
LDFLAGS = -static

#
# WINDOWS
#
# comment out below for non-windows builds
WIN = 1

ifdef WIN
#windows settings
LDLIBS += $(WIN_LIBS)
EXE_NAME = phoenix.exe

all: $(OBJECTS) $(OBJ_DIR)/resources.o
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(LDPATHS) $(LDLIBS) $(LDFLAGS) -o $(BIN_DIR)/$(EXE_NAME)

$(OBJ_DIR)/resources.o: resources.rc assets/eye.ico
	windres resources.rc $(OBJ_DIR)/resources.o
else
all: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(LDPATHS) $(LDLIBS) $(LDFLAGS) -o $(BIN_DIR)/$(EXE_NAME)
endif

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INC_PATHS) -c $< -o $@

.PHONY: clean
clean:
	rm -f build/*.*
	rm -f build/obj/*.*