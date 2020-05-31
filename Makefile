# EasySXB Makefile
#
# Static builds require that the fltk-1.3.5 source
# tree is local to this directory. Please run "make fltk"
# first to build the library before running "make".

# you MUST have libxft-dev installed before compiling FLTK on linux
# (otherwise you'll have ugly, non-resizable fonts)

ifeq ($(wildcard ./fltk-1.3.5),)
  PLATFORM=linux_dynamic
else
  PLATFORM=linux_static
endif

#PLATFORM=mingw32
#PLATFORM=mingw64

NAME="EasySXB "
#VERSION=$(shell git describe --always --dirty)
VERSION=$(shell git describe)

SRC_DIR=src
INCLUDE=-I$(SRC_DIR) -Ifltk-1.3.5

ifeq ($(PLATFORM),linux_dynamic)
  LIBS=$(shell fltk-config --ldflags)
  HOST=
  CXX=g++
  CXXFLAGS=-O3 -DPACKAGE_STRING=\"$(NAME)$(VERSION)\" $(INCLUDE)
  EXE=easysxb
endif

ifeq ($(PLATFORM),linux_static)
  LIBS=$(shell ./fltk-1.3.5/fltk-config --use-images --ldstaticflags)
  HOST=
  CXX=g++
  CXXFLAGS=-O3 -DPACKAGE_STRING=\"$(NAME)$(VERSION)\" $(INCLUDE)
  EXE=easysxb
endif

ifeq ($(PLATFORM),mingw32)
  LIBS=$(shell ./fltk-1.3.5/fltk-config --use-images --ldstaticflags)
  HOST=i686-w64-mingw32
  CXX=$(HOST)-g++
  CXXFLAGS=-O3 -static-libgcc -static-libstdc++ -DPACKAGE_STRING=\"$(NAME)$(VERSION)\" $(INCLUDE)
  LIBS+=-lgdi32 -lcomctl32 -lws2_32 -static -lpthread
  EXE=easysxb.exe
endif

ifeq ($(PLATFORM),mingw64)
  LIBS=$(shell ./fltk-1.3.5/fltk-config --use-images --ldstaticflags)
  HOST=x86_64-w64-mingw32
  CXX=$(HOST)-g++
  CXXFLAGS=-O3 -static-libgcc -static-libstdc++ -DPACKAGE_STRING=\"$(NAME)$(VERSION)\" $(INCLUDE)
  LIBS+=-lgdi32 -lcomctl32 -lws2_32 -static -lpthread
  EXE=easysxb.exe
endif

OBJ= \
  $(SRC_DIR)/Dialog.o \
  $(SRC_DIR)/DialogWindow.o \
  $(SRC_DIR)/Gui.o \
  $(SRC_DIR)/Separator.o \
  $(SRC_DIR)/Terminal.o

default: $(OBJ)
	$(CXX) -o ./$(EXE) $(SRC_DIR)/Main.cxx $(OBJ) $(CXXFLAGS) $(LIBS)

fltk:
	@cd ./fltk-1.3.5; \
	make clean; \
	./configure --host=$(HOST) --enable-localjpeg --enable-localzlib --enable-localpng --disable-xdbe; \
	make; \
	cd ..
	@echo "FLTK libs built!"

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cxx $(SRC_DIR)/%.H
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -f $(SRC_DIR)/*.o 
	@rm -f ./$(EXE)
	@echo "Clean!"

