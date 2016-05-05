# ZPatcher

A suite to Create and Apply patches to application and games.

It uses LZMA2 Compression, provided via [LZMA SDK 15.14][1].

It also uses a [Dirent interface for Windows][2].

## CreatePatch

Usage:
```CreatePatch.exe <old version directory> <new version directory> <output patch file>```

This utility compare two folders, find the differences between them and creates a "zpatch" file with this information.
Currently, it doesn't store any hash of the files changed nor deltas.

## ApplyPatch

Usage:
```ApplyPatch.exe <Patch file> <Directory to be patched>```

This utility receives a zpatch file as input and a directory to be patched.
It is mandatory that the file structure is the same as the file used to create the patch.
If a file is missing - even if it's supposed to be deleted in this version - it will fail.

If the patch applying process fails, it reverts all the changes processed until it fails.

Currently, there is no hash check of the files being updated.


## Final Considerations

I wanted to make this project platform independent, but it proved to be way harder than initially imaginated.
I wrote as close to as portable as I could, but currently, it should only work on Windows. If someone wants to tackle this portability problem, please, let me know!

Contributions to the project are welcome! Get in touch if you want to contribute.

I'm not sure how to properly give credit to libraries used in this project.
If it's done in a wrong way, I'd certainly appreciate some help on how to format and give credit in an appropriate way. :)

[1]: http://www.7-zip.org/sdk.html
[2]: https://github.com/tronkko/dirent
