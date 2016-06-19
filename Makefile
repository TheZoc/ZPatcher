CXX=g++
AR=ar
CXXFLAGS=-c -Wall -Wextra -std=c++14 -Wno-unused-parameter -Wno-unused-variable
CPPFLAGS=-I$(ZPATCHERLIBDIR) -Ilibs/LzmaLib/source -Ilibs/tinyxml2
LDFLAGS=-L$(LZMADIR)/out -L$(TINYXML2DIR)
ARFLAGS=-rvs

LZMADIR=./libs/LzmaLib/source
LZMALIB=$(LZMADIR)/out/liblzma.a

TINYXML2DIR=./libs/tinyxml2

LIBS=-llzma -lpthread -lcurl -ltinyxml2

# wxWidgets extra parameters
WXCONFIG_EXISTS:=$(shell command -v wx-config 2> /dev/null)
ifdef WXCONFIG_EXISTS
	CXXFLAGS+=$(shell wx-config --cxxflags)
	CPPFLAGS+=$(shell wx-config --cppflags)
	LIBS+=$(shell wx-config --libs std,webview)
endif

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

# VisualCreatePatch specific source files
VISUALCREATEPATCHDIR=VisualCreatePatch
VISUALCREATEPATCHSOURCES=$(wildcard $(VISUALCREATEPATCHDIR)/*.cpp)
VISUALCREATEPATCHOBJECTS=$(addprefix obj/$(VISUALCREATEPATCHDIR)/, $(notdir $(VISUALCREATEPATCHSOURCES:.cpp=.o)))

# ZUpdater specific source files
ZLAUNCHERRDIR=ZLauncher
ZLAUNCHERSOURCES=$(wildcard $(ZLAUNCHERRDIR)/*.cpp)
ZLAUNCHEROBJECTS=$(addprefix obj/$(ZLAUNCHERRDIR)/, $(notdir $(ZLAUNCHERSOURCES:.cpp=.o)))

# ZUpdater specific source files
ZUPDATERDIR=ZUpdater
ZUPDATERSOURCES=$(wildcard $(ZUPDATERDIR)/*.cpp)
ZUPDATEROBJECTS=$(addprefix obj/$(ZUPDATERDIR)/, $(notdir $(ZUPDATERSOURCES:.cpp=.o)))

# A directory creation utility
create_output_dir=@mkdir -p $(@D)

.PHONY: all lzma tinyxml2 clean

# If there is no wx-config file, assume wxWidgets is missing and only build the command-line utilities
ifndef WXCONFIG_EXISTS
all: lzma tinyxml2 out/ZPatcherLib.a CreatePatch ApplyPatch ZUpdater
else
all: lzma tinyxml2 out/ZPatcherLib.a CreatePatch ApplyPatch ZUpdater VisualCreatePatch ZLauncher
endif
	@echo all done.

libs: lzma tinyxml2 out/ZPatcherLib.a

lzma:
	@ $(MAKE) -C $(LZMADIR)

tinyxml2:
	@ $(MAKE) -C $(TINYXML2DIR)

CreatePatch: libs out/CreatePatch

ApplyPatch: libs out/ApplyPatch

VisualCreatePatch: libs out/VisualCreatePatch

ZLauncher: libs out/ZLauncher
	@ cp -R ./ZLauncher/ZLauncherRes ./out/ZLauncherRes/

ZUpdater: libs out/ZUpdater

clean:
	@ rm -rf obj/
	@ rm -rf out/
	@ $(MAKE) -C $(LZMADIR) clean
	@ $(MAKE) -C $(TINYXML2DIR) clean

# CreatePatch executable
out/CreatePatch: $(CREATEPATCHOBJECTS) out/ZPatcherLib.a
	$(create_output_dir)
	$(CXX) $(LDFLAGS) $^ -o $@ $(LIBS)

$(CREATEPATCHOBJECTS): obj/$(CREATEPATCHDIR)/%.o: $(CREATEPATCHDIR)/%.cpp
	$(create_output_dir)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $< -o $@

# ApplyPatch executable
out/ApplyPatch: $(APPLYPATCHOBJECTS) out/ZPatcherLib.a
	$(create_output_dir)
	$(CXX) $(LDFLAGS) $^ -o $@ $(LIBS)

$(APPLYPATCHOBJECTS): obj/$(APPLYPATCHDIR)/%.o: $(APPLYPATCHDIR)/%.cpp
	$(create_output_dir)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $< -o $@

# VisualCreatePatch executable
out/VisualCreatePatch: $(VISUALCREATEPATCHOBJECTS) out/ZPatcherLib.a
	$(create_output_dir)
	$(CXX) $(LDFLAGS) $^ -o $@ $(LIBS)

$(VISUALCREATEPATCHOBJECTS): obj/$(VISUALCREATEPATCHDIR)/%.o: $(VISUALCREATEPATCHDIR)/%.cpp
	$(create_output_dir)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $< -o $@

# ZLauncher executable
out/ZLauncher: $(ZLAUNCHEROBJECTS) out/ZPatcherLib.a
	$(create_output_dir)
	$(CXX) $(LDFLAGS) $^ -o $@ $(LIBS)

$(ZLAUNCHEROBJECTS): obj/$(ZLAUNCHERRDIR)/%.o: $(ZLAUNCHERRDIR)/%.cpp
	$(create_output_dir)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $< -o $@

# ZUpdater executable
out/ZUpdater: $(ZUPDATEROBJECTS) out/ZPatcherLib.a
	$(create_output_dir)
	$(CXX) $(LDFLAGS) $^ -o $@ $(LIBS)

$(ZUPDATEROBJECTS): obj/$(ZUPDATERDIR)/%.o: $(ZUPDATERDIR)/%.cpp
	$(create_output_dir)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $< -o $@

# ZPatcherLib Static Library
out/ZPatcherLib.a: $(ZPATCHERLIBOBJECTS)
	$(create_output_dir)
	$(AR) $(ARFLAGS) $@ $^

$(ZPATCHERLIBOBJECTS): obj/$(ZPATCHERLIBDIR)/%.o: $(ZPATCHERLIBDIR)/%.cpp
	$(create_output_dir)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $< -o $@
