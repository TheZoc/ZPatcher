CC=g++
AR=ar
CXXFLAGS=-c -Wall -Wextra -std=c++14 -Wno-unused-parameter -Wno-unused-variable
CPPFLAGS=-I$(ZPATCHERLIBDIR) -Ilibs/LzmaLib/source
LDFLAGS= -L$(LZMADIR)/out
ARFLAGS=-rvs

LZMADIR=./libs/LzmaLib/source
LZMALIB=$(LZMADIR)/out/liblzma.a

LIBS=-llzma -lpthread

# ZPatcherLib files
ZPATCHERLIBDIR=ZPatcherLib
ZPATCHERLIBSOURCES=$(wildcard $(ZPATCHERLIBDIR)/*.cpp)
ZPATCHERLIBOBJECTS=$(addprefix obj/$(ZPATCHERLIBDIR)/,$(notdir $(ZPATCHERLIBSOURCES:.cpp=.o)))

# CreatePatch specific source files
CREATEPATCHDIR=CreatePatch
CREATEPATCHSOURCES=$(wildcard $(CREATEPATCHDIR)/*.cpp)
CREATEPATCHOBJECTS=$(addprefix obj/$(CREATEPATCHDIR)/, $(notdir $(CREATEPATCHSOURCES:.cpp=.o)))

# ApplyPatch specific source files
APPLYPATCHDIR=ApplyPatch
APPLYPATCHSOURCES=$(wildcard $(APPLYPATCHDIR)/*.cpp)
APPLYPATCHOBJECTS=$(addprefix obj/$(APPLYPATCHDIR)/, $(notdir $(APPLYPATCHSOURCES:.cpp=.o)))

# A directory creation utility
create_output_dir=@mkdir -p $(@D)

.PHONY: all lzma clean

all: lzma out/ZPatcherLib.a out/CreatePatch out/ApplyPatch
	@echo all done.

lzma:
	@ $(MAKE) -C $(LZMADIR)

clean:
	@ rm -rf obj/
	@ rm -rf out/
	@ $(MAKE) -C $(LZMADIR) clean

# CreatePatch executable
out/CreatePatch: $(CREATEPATCHOBJECTS) out/ZPatcherLib.a
	$(create_output_dir)
	$(CC) $(LDFLAGS) $^ -o $@ $(LIBS)

$(CREATEPATCHOBJECTS): obj/$(CREATEPATCHDIR)/%.o: $(CREATEPATCHDIR)/%.cpp
	$(create_output_dir)
	$(CC) $(CXXFLAGS) $(CPPFLAGS) $< -o $@

# ApplyPatch executable
out/ApplyPatch: $(APPLYPATCHOBJECTS) out/ZPatcherLib.a
	$(create_output_dir)
	$(CC) $(LDFLAGS) $^ -o $@ $(LIBS)

$(APPLYPATCHOBJECTS): obj/$(APPLYPATCHDIR)/%.o: $(APPLYPATCHDIR)/%.cpp
	$(create_output_dir)
	$(CC) $(CXXFLAGS) $(CPPFLAGS) $< -o $@

# ZPatcherLib Static Library
out/ZPatcherLib.a: $(ZPATCHERLIBOBJECTS)
	$(create_output_dir)
	$(AR) $(ARFLAGS) $@ $^

$(ZPATCHERLIBOBJECTS): obj/$(ZPATCHERLIBDIR)/%.o: $(ZPATCHERLIBDIR)/%.cpp
	$(create_output_dir)
	$(CC) $(CXXFLAGS) $(CPPFLAGS) $< -o $@
