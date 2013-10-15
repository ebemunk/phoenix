include Makefile-variables.mk

all: phoenix.o functions.o resources.o
	$(CC) phoenix.o functions.o resources.o $(CFLAGS) $(LDFLAGS) -o $(EXE_NAME)

phoenix.o: phoenix.cpp functions.o
	$(CC) -c phoenix.cpp $(INC_PATHS) $(CFLAGS)

functions.o: functions.cpp
	$(CC) -c functions.cpp $(INC_PATHS) $(CFLAGS)

resources.o: resources.rc favicon.ico
	windres resources.rc resources.o

.PHONY: clean
clean:
	rm *.o phoenix.exe phoenix-dyn.exe

speedtests: functions.cpp
	$(CC) -c speedtests.cpp $(INC_PATHS) $(CFLAGS)
	$(CC) speedtests.o functions.o $(CFLAGS) $(LDFLAGS) -o st.exe