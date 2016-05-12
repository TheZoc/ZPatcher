# ZPatcher

A suite to Create and Apply patches to application and games.

ZPatcher uses a few external libraries to aid in it's functions:

- [libcurl][4]
- [Dirent interface for Windows][3] for directory navigation under Windows
- [LZMA SDK 15.14][1] provides the LZMA2 compression algorithm.
- [TinyXML2][2] for XML parsing
- [MD5][5] implementation by Frank Thilo

CreatePatch and ApplyPatch uses ZPatcherLib, which uses [Dirent][3] and [LZMA SDK][1].
ZUpdater also uses [libcurl][4] and [TinyXML2][2].


## Compiling

Run the file ```.\libs\curl\projects\generate.bat``` to generate libcurl required files.
Currently, you will need to upgrade TinyXML2

Oopen ```ZPatcher.sln``` solution file and compile it. Make sure Configuration and Platform are correct!

The executables will be in the ```_Output/``` directory.

## Applications - Offline Patching

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
Also, currently, there is no version tracking mechanism in place.

## Applications - Online Patching

### ZUpdater

Usage: Just run the program, it will do it's magic ;)

Setup:

You will need a XML containing the update data. Instructions should be added in the next few commits.


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
[2]: https://github.com/leethomason/tinyxml2
[3]: https://github.com/tronkko/dirent
[4]: https://curl.haxx.se/libcurl/
[5]: http://www.bzflag.org