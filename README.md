# ZPatcher

A suite to Create and Apply patches to application and games.

ZPatcher uses a few external libraries to aid in it's functions:

- [LZMA SDK 15.14][1] provides the LZMA2 compression algorithm.
- [wxWidgets][2] is used to provide Graphical User Interface.
- [Dirent interface for Windows][3] for directory navigation under Windows

## Compiling

To compile you will need to [get wxWidgets here][4].
This project currently uses version [3.1.0][5], but newer versions should work without problems.

For Visual Studio 2015, the files you are looking for are ```wxWidgets-x.y.z-headers.7z``` and either ```wxMSW-x.y.z_vc120_Dev.7z``` or ```wxMSW-x.y.z_vc140_x64_Dev.7z``` where ```x.y.z``` is the version of wxWidgets you are using.

You must extract those files in the ```libs/wxWidgets/``` directory, only having a top level ```include/``` and either ```vc140_x64_dll/``` or ```vc140_dll/``` directory.

To illustrate better, for the 64 bits version:
```
+ libs/
|---+ wxwidgets/
    |---- include/
	|---- vc140_x64_dll/
```

Open ZPatcher solution file and compile it. Make sure Configuration and Platform are correct!

The executables will be in the ```_Output/``` directory.

## Applications

### CreatePatch

Usage:
```CreatePatch.exe <old version directory> <new version directory> <output patch file>```

This utility compare two folders, find the differences between them and creates a "zpatch" file with this information.
Currently, it doesn't store any hash of the files changed nor deltas.

### ApplyPatch

Usage:
```ApplyPatch.exe <Patch file> <Directory to be patched>```

This utility receives a zpatch file as input and a directory to be patched.
It is mandatory that the file structure is the same as the file used to create the patch.
If a file is missing - even if it's supposed to be deleted in this version - it will fail.

If the patch applying process fails, it reverts all the changes processed until it fails.

Currently, there is no hash check of the files being updated.

## Patch Logging

There is a very simple (and extremely verbose, for now), log system in the patcher.
It will create a Logs/ folder on it's base directory and output all the operations (including failures) to it.

## Final Considerations

I wanted to make this project platform independent, but it proved to be way harder than initially imaginated.
I wrote as close to as portable as I could, but currently, it should only work on Windows. If someone wants to tackle this portability problem, please, let me know!

Contributions to the project are welcome! Get in touch if you want to contribute.

I'm not sure how to properly give credit to libraries used in this project.
If it's done in a wrong way, I'd certainly appreciate some help on how to format and give credit in an appropriate way. :)

[1]: http://www.7-zip.org/sdk.html
[2]: http://www.wxwidgets.org/
[3]: https://github.com/tronkko/dirent
[4]: https://github.com/wxWidgets/wxWidgets/releases
[5]: https://github.com/wxWidgets/wxWidgets/releases/tag/v3.1.0
