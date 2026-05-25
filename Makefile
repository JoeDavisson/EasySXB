# EasySXB Makefile

# The official FLTK source tree must be in this directory.
# run "make fltklib", then "make"

# libxft-dev should be installed before compiling FLTK on linux
# (otherwise you'll have ugly, non-resizable fonts)

FLTK_DIR=fltk-1.4.5
PLATFORM=linux
#PLATFORM=mingw32
#PLATFORM=mingw64

VERSION=0.1.7
SRC_DIR=src
INCLUDE=-I$(SRC_DIR) -I$(FLTK_DIR)
LIBS=$(shell ./$(FLTK_DIR)/fltk-config --use-images --ldstaticflags)

ifeq ($(PLATFORM),linux)
  HOST=
  CXX=g++
  CXXFLAGS= -O3 -Wall -Wunused-parameter -DPACKAGE_STRING=\"$(VERSION)\" $(INCLUDE)
  EXE=easysxb
endif

ifeq ($(PLATFORM),mingw32)
  HOST=i686-w64-mingw32
  CXX=$(HOST)-g++
  CXXFLAGS= -O3 -Wall -static-libgcc -static-libstdc++ -DPACKAGE_STRING=\"$(VERSION)\" $(INCLUDE)
  LIBS+=-lgdi32 -lcomctl32 -static -lpthread
  EXE=easysxb.exe
endif

ifeq ($(PLATFORM),mingw64)
  HOST=x86_64-w64-mingw32
  CXX=$(HOST)-g++
  CXXFLAGS= -O3 -Wall -static-libgcc -static-libstdc++ -DPACKAGE_STRING=\"$(VERSION)\" $(INCLUDE)
  LIBS+=-lgdi32 -lcomctl32 -static -lpthread
  EXE=easysxb.exe
endif

OBJ= \
  $(SRC_DIR)/Dialog.o \
  $(SRC_DIR)/DialogWindow.o \
  $(SRC_DIR)/Group.o \
  $(SRC_DIR)/Gui.o \
  $(SRC_DIR)/Separator.o \
  $(SRC_DIR)/Terminal.o

# build program
default: $(OBJ)
	$(CXX) -o ./$(EXE) $(SRC_DIR)/Main.cxx $(OBJ) $(CXXFLAGS) $(LIBS)

# build fltk
fltklib:
	cd ./$(FLTK_DIR); \
	make clean; \
	./configure --host=$(HOST) --enable-usecairo --enable-pango --enable-xft --enable-localjpeg --enable-localzlib --enable-localpng --disable-xdbe; \
	make -j12; \
	cd ..; \
	echo "FLTK lib built.";

# remove object files
clean:
	@rm -f $(SRC_DIR)/*.o 
	@rm -f ./easysxb
	@rm -f ./easysxb.exe
	@echo "Clean."

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cxx $(SRC_DIR)/%.H
	$(CXX) $(CXXFLAGS) -c $< -o $@

