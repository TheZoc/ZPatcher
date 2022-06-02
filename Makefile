CXX=g++
AR=ar
CXXFLAGS=-c -Wall -Wextra -std=c++14 -Wno-unused-parameter -Wno-unused-variable -D_FILE_OFFSET_BITS=64
CPPFLAGS=-I$(ZPATCHERLIBDIR) -Ilibs/LzmaLib/source
LDFLAGS=-L$(LZMADIR)/out
ARFLAGS=-rvs

LZMADIR=./libs/LzmaLib/source
LZMALIB=$(LZMADIR)/out/liblzma.a

WXWIDGETSDIR=./libs/wxWidgets

LIBS=-llzma -lpthread -lcurl

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

.PHONY: all lzma clean

all: lzma out/ZPatcherLib.a wxWidgets CreatePatch ApplyPatch ZUpdater VisualCreatePatch ZLauncher
	@echo all done.

libs: lzma out/ZPatcherLib.a

lzma:
	@ $(MAKE) -C $(LZMADIR)

wxWidgets:
	(if test ! -f $(WXWIDGETSDIR)/Makefile ; then cd $(WXWIDGETSDIR) && ./configure --disable-shared ; fi)
	@ $(MAKE) -C $(WXWIDGETSDIR)

wxWidgetsVariables: wxWidgets
	$(eval CXXFLAGS+=$(shell $(WXWIDGETSDIR)/wx-config --cxxflags))
	$(eval CPPFLAGS+=$(shell $(WXWIDGETSDIR)/wx-config --cppflags))
	$(eval LIBS+=$(shell $(WXWIDGETSDIR)/wx-config --libs std,webview))

CreatePatch: libs out/CreatePatch

ApplyPatch: libs out/ApplyPatch

VisualCreatePatch: wxWidgets wxWidgetsVariables libs out/VisualCreatePatch

ZLauncher: wxWidgets wxWidgetsVariables libs out/ZLauncher
	@ cp -R ./ZLauncher/ZLauncherRes ./out/ZLauncherRes/
	@ cp -n ./ZLauncher/ZLauncher.xml ./out/ | true

ZUpdater: libs out/ZUpdater

clean:
	@ rm -rf obj/
	@ rm -rf out/
	@ $(MAKE) -C $(LZMADIR) clean
	@ $(MAKE) -C $(WXWIDGETSDIR) clean

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
