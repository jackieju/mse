# Project: se

headers = $(shell echo ./*.h )
srcs = $(shell echo ./*.cpp )
objs = $(srcs:.cpp=.o)

CC   = g++ 


OBJ  = $(objs)
BIN  = coco
CFLAGS = -g -fPIC  -O2 

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after


clean: clean-custom
	rm -f $(OBJ) $(BIN)

DLLWRAP=g++


$(BIN): $(OBJ)
	g++ $(OBJ) -o coco
#	$(DLLWRAP) -shared -ldl  $(OBJ)  -o $(BIN)
#	$(DLLWRAP) -shared -Wl,-soname,$(SOBIN)  $(OBJ)  -o ./bin/$(BIN) -lc

