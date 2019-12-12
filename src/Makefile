TARGET = arch
BIN_PATH = $(PWD)/bin
SRC_PATH = $(PWD)
LIB_PATH = $(PWD)/lib

.PHONY: clean

clean:
	rm -rf $(BIN_PATH)/$(TARGET) $(LIB_PATH)/*.o
archiver.o: $(SRC_PATH)/archiver.c
	gcc -c -o $(LIB_PATH)/archiver.o $(SRC_PATH)/archiver.c
main.o: $(SRC_PATH)/main.c
	gcc -c -o $(LIB_PATH)/main.o $(SRC_PATH)/main.c
lib:
	if test -d lib;then echo "lib already exists";else mkdir lib;fi
$(TARGET): lib main.o archiver.o
	if test -d bin;then echo "bin already exists";else mkdir bin;fi
	gcc -o $(BIN_PATH)/$(TARGET) $(LIB_PATH)/main.o $(LIB_PATH)/archiver.o
