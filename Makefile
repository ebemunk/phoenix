include Makefile-variables-win.mk

ifdef WIN
all: $(BUILD_DIR)/phoenix.o $(BUILD_DIR)/functions.o $(BUILD_DIR)/resources.o
	$(CC) $(BUILD_DIR)/phoenix.o $(BUILD_DIR)/functions.o $(BUILD_DIR)/resources.o $(CFLAGS) $(LDFLAGS) -o $(BUILD_DIR)/$(EXE_NAME)

$(BUILD_DIR)/resources.o: resources.rc assets/eye.ico
	windres resources.rc $(BUILD_DIR)/resources.o
else
all: $(BUILD_DIR)/phoenix.o $(BUILD_DIR)/functions.o
	$(CC) $(BUILD_DIR)/phoenix.o $(BUILD_DIR)/functions.o $(CFLAGS) $(LDFLAGS) -o $(BUILD_DIR)/$(EXE_NAME)
endif

$(BUILD_DIR)/phoenix.o: phoenix.cpp $(BUILD_DIR)/functions.o
	$(CC) -c phoenix.cpp $(INC_PATHS) $(CFLAGS) -o $(BUILD_DIR)/phoenix.o

$(BUILD_DIR)/functions.o: functions.cpp
	$(CC) -c functions.cpp $(INC_PATHS) $(CFLAGS) -o $(BUILD_DIR)/functions.o

.PHONY: clean
clean:
	rm $(BUILD_DIR)/*

.PHONY: speedtests
speedtests: functions.cpp
	$(CC) -c speedtests.cpp $(INC_PATHS) $(CFLAGS) -o $(BUILD_DIR)/speedtests.o
	$(CC) speedtests.o functions.o $(CFLAGS) $(LDFLAGS) -o $(BUILD_DIR)/st.exe