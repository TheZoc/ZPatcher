ZPatcher Setup and Compiling
===========================

When cloning the repository, make sure to clone with all the submodules.
One way to do this is to run:

```
git clone --recurse-submodules https://github.com/TheZoc/ZPatcher.git
```

After the clone is completed, follow the appropriate section for your Operating System.

Please note that the visual applications were developed with `wxWidgets 3.1` and were not tested with older versions.

Windows
-------

### Automatic dependencies setup

Run the `setup-external-dependencies.ps1` PowerShell script.
Keep in mind this script only works for Visual Studio 2015 right now.

This will download all the required `wxWidgets` files, put them in the appropriate directories and generate `libcurl` required project files.

For backwards compatibility, the project solution currently uses a Visual Studio 2015 solution, supporting up to Windows XP.
You can open it with the latest version of Visual Studio and upgrade it and it will also work. Make sure you setup the Windows SDK version to the latest one as well.

Please, check the section below for information on how to generate the libcurl projects for all Visual Studio versions.

### Manual dependencies setup

1. Run the file `.\libs\curl\projects\generate.bat` to generate `libcurl` required files.
2. Download and add wxWidget 3.1+ required files to `.\libs\wxWidgets` directory. Please, note, wxWidgets subdirectories should be `.\libs\wxWidgets\lib\vc_x64_dll` and `.\libs\wxWidgets\lib\vc_dll`. For a list of necessary files, check `setup-external-dependencies.ps1`.


macOS
-----

1. Install the required dependencies: `libcurl` and `wxWidgets`. It's recommended to use [homebrew][1] to do that.
2. Run `make`
3. The executables will be on the `out/` directory.

Please note that the `Makefile` uses the `wx-config` utility to build the visual applications of the ZPatcher suite. Make sure this utility is available if you plan to build these applications.


Linux
-----

1. Install the requried dependencies using your favourite package manager: `libcurl` and `wxWidgets`.
2. Run `make`
3. The executables will be on the `out/` directory.


Please note that the `Makefile` uses the `wx-config` utility to build the visual applications of the ZPatcher suite. Make sure this utility is available if you plan to build these applications.


[1]: http://brew.sh/