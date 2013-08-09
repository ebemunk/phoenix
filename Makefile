include Makefile-variables.mk

all: phoenix.o functions.o resources.o
	$(CC) phoenix.o functions.o resources.o $(CFLAGS) $(LDFLAGS) -o $(EXE_NAME)

phoenix.o: phoenix.cpp functions.o
	$(CC) -c phoenix.cpp $(INC_PATHS)

functions.o: functions.cpp
	$(CC) -c functions.cpp -I$(OCV_INC)

resources.o: resources.rc
	windres resources.rc resources.o

.PHONY: clean
clean:
	rm *.o phoenix.exe phoenix-dyn.exe