CC=g++
AR=ar
CXXFLAGS=-c -Wall -std=c++14
CPPFLAGS=-I$(ZPATCHERLIBDIR) -Ilibs/LzmaLib/source
LDFLAGS=
ARFLAGS=-rvs

# ZPatcherLib files
ZPATCHERLIBDIR=ZPatcherLib
ZPATCHERLIBSOURCES=$(wildcard $(ZPATCHERLIBDIR)/*.cpp)
ZPATCHERLIBOBJECTS=$(addprefix obj/$(ZPATCHERLIBDIR)/,$(notdir $(ZPATCHERLIBSOURCES:.cpp=.o)))

# CreatePatch specific source files
CREATEPATCHDIR=CreatePatch
CREATEPATCHSOURCES=$(wildcard $(CREATEPATCHDIR)/*.cpp)
CREATEPATCHOBJECTS=$(addprefix obj/$(CREATEPATCHDIR)/, $(notdir $(CREATEPATCHSOURCES:.cpp=.o)))

# ApplyPatch specific source files
APPLYPATCHDIR=ApplyPatchDir
APPLYPATCHSOURCES=$(wildcard $(APPLYPATCHDIR)/*.cpp)
APPLYPATCHOBJECTS=$(addprefix obj/$(APPLYPATCHDIR)/, $(notdir $(APPLYPATCHSOURCES:.cpp=.o)))

# A directory creation utility
create_output_dir=@mkdir -p $(@D)

all: out/ZPatcherLib.a out/CreatePatch out/ApplyPatch
	@echo all done.

clean:
	rm -rf obj/
	rm -rf out/

# CreatePatch executable
out/CreatePatch: $(CREATEPATCHOBJECTS) out/ZPatcherLib.a
	$(create_output_dir)
	$(CC) $(LDFLAGS) $^ -o $@

$(CREATEPATCHOBJECTS): obj/$(CREATEPATCHDIR)/%.o: $(CREATEPATCHDIR)/%.cpp
	$(create_output_dir)
	$(CC) $(CXXFLAGS) $(CPPFLAGS) $< -o $@

# ApplyPatch executable
out/ApplyPatch: $(APPLYPATCHOBJECTS) out/ZPatcherLib.a
	$(create_output_dir)
	$(CC) $(LDFLAGS) $^ -o $@

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
