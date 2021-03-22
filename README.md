[![Build Status](https://www.travis-ci.com/TheZoc/ZPatcher.svg?branch=master)](https://www.travis-ci.com/TheZoc/ZPatcher)
[![Build status](https://ci.appveyor.com/api/projects/status/i6kr70f4x40bero5/branch/master?svg=true)](https://ci.appveyor.com/project/TheZoc/zpatcher/branch/master)

# ZPatcher

The ZPatcher suite is a set of applications developed to Create and Apply patches to both applicatoin and games.

![ZLauncher Sample Screenshot](https://raw.githubusercontent.com/TheZoc/ZPatcher/master/images/ZLauncher.png)

Features:
* Multiplatform, currently supporting Windows, OSX and Linux.
* Open Source
* Easy to Create patch files for your game/application
* Skinnable
* Command line tools available to be easily included in deployment scripts
* Client-Side automatic Download-and-Apply patching process
* File backup when applying a patch, reducing the chances of a broken build on clients
* (Launcher Only) Display the Change Log for your patches

## Setup and Compiling

### On Windows

There are two ways to setup the project dependencies on Windows, using a script to setup them automatically and manually.

#### Automatic dependencies setup

Run the `setup-external-dependencies.ps1` PowerShell script.
Keep in mind this script only works for Visual Studio 2015 right now.

This will download all the required `wxWidgets` files, put them in the appropriate directories and generate `libcurl` required project files.

#### Manual dependencies setup

1. Run the file `.\libs\curl\projects\generate.bat` to generate `libcurl` required files.
2. Download and add  wxWidget required files to `.\libs\wxWidgets` directory. Please, note, wxWidgets subdirectories should be `.\libs\wxWidgets\lib\vc140_x64_dll` and `.\libs\wxWidgets\lib\vc140_dll`.

#### Compiling under Windows

After `libcurl` project files are generated and `wxWidgets` required files are in place:

1. Open `ZPatcher.sln` solution file and compile it. Make sure Configuration and Platform are correct!
2. The executables will be in the `_Output/` directory.

### On OSX

1. Install the required dependencies: `libcurl` and `wxWidgets`. It's recommended to use [homebrew][7] to do that.
2. Run `make`
3. The executables will be on the `out/` directory.

Please note, the `Makefile` uses the `wx-config` utility to build the visual applications of the ZPatcher suite. Make sure this utility is available if you plan to build these applications.

### On Linux

1. Install the requried dependencies using your favourite package manager: `libcurl` and `wxWidgets`.
2. Run `make`
3. The executables will be on the `out/` directory.

Please note that the visual applications were developed with `wxWidgets 3.1` and was not tested with older versions.
Also, the `Makefile` uses the `wx-config` utility to build the visual applications of the ZPatcher suite. Make sure this utility is available if you plan to build these applications.

## Applications

### Patch creation utilites

Both `CreatePatch` and `VisualCreatePatch` can be used to create the patch files.
* `CreatePatch` is better suited to create patches on batch files and scripts, being a command-line application.
* `VisualCreatePatch` is better suited to create a patch manually, since it displays a window with the patch creation process in a user friendly interface. It is also faster than the command-line counterpart.

To create a patch with `CreatePatch`, the usage is:
`CreatePatch.exe <old version directory> <new version directory> <output patch file>`

To create a patch with `VisualCreatePatch`, the usage is:
`VisualCreatePatch.exe -o <old version directory> -n <new version directory> -p <output patch file>`

The command line input is slightly different, since `VisualCreatePatch` uses wxWidgets tools to process the command line input.

Currently, both applications must be run using a command line, the `VisualCreatePatch` user interface is currently pending.
Note: You can always submit a pull request for it! Make the command line input optional and add dialogs for folder inputs.

## Patch Applying utilities

### ApplyPatch

Usage:
`ApplyPatch.exe <Patch file> <Directory to be patched with trailing slash>`

This utility receives a zpatch file as input and a directory to be patched.
It is mandatory that the file structure is the same as the file used to create the patch.
If a file is missing - even if it's supposed to be deleted during the patching process - it will fail.

If the patch applying process fails, it reverts all the changes processed until it fails.

Currently, ApplyPatch do not have a version tracking mechanism in place.

### ZUpdater

Command line tool to fetch the update information from an URL, download the required patches and apply them.

It is required to setup a webserver with a XML file containing the update data.
After it's configured, you will need to edit the necessary data in `ZUpdater/main.cpp`.
Compile and run it, the magic will happen.

It is possible to do a self-update on Windows, you just need to add an executable name with an extra "a" on it's name.
For example, to update `ZUpdater.exe`, add a file called `ZUpdatera.exe` on the same directory.
This file can be delivered using a `.zpatch` file.

### ZLauncher

Visual application that feches the update information from an URL, download the required patches and apply them.

The setup is exactly the same as the `ZUpdater` one, but this one will also display the changes for each patch.

#### XML Structure for the Update information

A [sample XML][8] is included in the `tests/` directory. You can see the structure of it there.

## Patch Logging

There is a very simple (and extremely verbose, for now), log system in the patcher.
It will create a Logs/ folder on it's base directory and output all the operations (including failures) to it.
Pull requests for improving the log system are welcome!

## Missing features

There are a number of planned, but missing features. These include, but are not limited to:

- Add hash check of the files to be updated by a patch.
- Add support for binary file deltas. Currently considering using [minibsdiff][6], but suggestions are welcome.
- Patch merger (i.e. Given two incremental patch files, merge them in a single patch file).
- Update the file format so it includes the compressed block size, avoiding to use the "file ended" flag from LZMA.

Please note these features are not listed in order of priority. Also, there isn't a timeframe for those features to be added.
Pull requests are welcome!

## Third Party libraries

ZPatcher uses a few external libraries to aid in it's functions:

- [libcurl][4]
- [Dirent interface for Windows][3] for directory navigation under Windows
- [LZMA SDK 16.02][1] provides the LZMA2 compression algorithm.
- [RapidXML][2] for XML parsing
- [MD5][5] implementation by L. Peter Deutsch

CreatePatch and ApplyPatch uses ZPatcherLib, which uses [Dirent][3] and [LZMA SDK][1].

ZUpdater also uses [libcurl][4] and [RapidXML][2].

## Final Considerations

It is my intent to make this project as platform independent as possible. As of this writing, both offline utilities are platform independent, and the online utility should follow soon.
I'd love to have a visual patcher for this project, but my current available time doesn't allow

Contributions to the project are welcome! Get in touch if you want to contribute and be sure to send pull requests!

Last but not least, I'm not sure how to properly give credit to libraries used in this project.
If it's done in a wrong way, I'd certainly appreciate some help on how to format and give credit in an appropriate way. :)

[1]: http://www.7-zip.org/sdk.html
[2]: http://rapidxml.sourceforge.net/
[3]: https://github.com/tronkko/dirent
[4]: https://curl.haxx.se/libcurl/
[5]: https://sourceforge.net/projects/libmd5-rfc/files/
[6]: https://github.com/thoughtpolice/minibsdiff
[7]: http://brew.sh/
[8]: https://github.com/TheZoc/ZPatcher/blob/master/tests/zpatcher_test.xml
