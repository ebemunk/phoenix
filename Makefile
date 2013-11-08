include Makefile-variables.mk

all: $(BUILD_DIR)/phoenix.o $(BUILD_DIR)/functions.o $(BUILD_DIR)/resources.o
	$(CC) $(BUILD_DIR)/phoenix.o $(BUILD_DIR)/functions.o $(BUILD_DIR)/resources.o $(CFLAGS) $(LDFLAGS) -o $(BUILD_DIR)/$(EXE_NAME)

$(BUILD_DIR)/phoenix.o: phoenix.cpp $(BUILD_DIR)/functions.o
	$(CC) -c phoenix.cpp $(INC_PATHS) $(CFLAGS) -o $(BUILD_DIR)/phoenix.o

$(BUILD_DIR)/functions.o: functions.cpp
	$(CC) -c functions.cpp $(INC_PATHS) $(CFLAGS) -o $(BUILD_DIR)/functions.o

$(BUILD_DIR)/resources.o: resources.rc eye.ico
	windres resources.rc $(BUILD_DIR)/resources.o

.PHONY: clean
clean:
	rm $(BUILD_DIR)/*.o $(BUILD_DIR)/phoenix.exe

speedtests: functions.cpp
	$(CC) -c speedtests.cpp $(INC_PATHS) $(CFLAGS) -o $(BUILD_DIR)/speedtests.o
	$(CC) speedtests.o functions.o $(CFLAGS) $(LDFLAGS) -o $(BUILD_DIR)/st.exe