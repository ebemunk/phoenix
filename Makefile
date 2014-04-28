#
# COMPILER CONFIG
#
#compiler details
CXX = g++
CXXFLAGS = -g -std=c++0x -O3

#project structure
OBJ_DIR = build/obj
BIN_DIR = build

#source files and corresponding objects
SOURCES = debugger.cpp functions.cpp phoenix.cpp
OBJECTS = $(SOURCES:%.cpp=$(OBJ_DIR)/%.o)

#header file locations
OCV_INC = C:\opencv_2_4_6\build\include
BOOST_INC = C:\boost_1_54_0
INC_PATHS = -isystem$(OCV_INC) -isystem$(BOOST_INC)

#
# LINKER CONFIG
#
#used libraries
OCV_LIBS = -lopencv_highgui -lopencv_imgproc -lopencv_core
BOOST_LIBS = -lboost_program_options -lboost_filesystem -lboost_system
WIN_DEPS = -lzlib -llibjpeg -llibtiff -llibpng -lcomctl32 -lgdi32
LINUX_DEPS = `pkg-config opencv --libs`

#linker options
LDLIBS = $(OCV_LIBS) $(BOOST_LIBS)
LDFLAGS =

#
# WINDOWS
#
# comment out below for non-windows builds
WIN = 1

ifdef WIN
#windows settings
EXE_NAME = phoenix.exe
LDLIBS += $(WIN_DEPS)
LDFLAGS = -static

all: $(OBJECTS) $(OBJ_DIR)/resources.o
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(OBJ_DIR)/resources.o $(LDLIBS) $(LDFLAGS) -o $(BIN_DIR)/$(EXE_NAME)

$(OBJ_DIR)/resources.o: resources.rc assets/eye.ico
	windres resources.rc $(OBJ_DIR)/resources.o
else
#linux settings
EXE_NAME = phoenix
LDLIBS += $(LINUX_DEPS)
#set rpath on linux because of shared lib build
LDFLAGS = -Wl,-rpath,\$$ORIGIN/libs,-z,origin

all: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(LDLIBS) $(LDFLAGS) -o $(BIN_DIR)/$(EXE_NAME)
endif

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INC_PATHS) -c $< -o $@

dev: $(OBJ_DIR)/debugger.o
	$(CXX) $(CXXFLAGS) $(INC_PATHS) -c dev.cpp -o $(OBJ_DIR)/dev.o
	$(CXX) $(CXXFLAGS) $(OBJ_DIR)/dev.o $(OBJ_DIR)/debugger.o $(LDLIBS) $(LDFLAGS) -o $(BIN_DIR)/dev.exe

.PHONY: clean
clean:
	rm -f build/*.*
	rm -f build/obj/*.*
