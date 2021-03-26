Patch File Format
=================

The zpatch file contains all the data required to upgrade a software.
The data is stored in a structured way, so it's easy to access and read.


File Structure
--------------

```
Header [1 entry]:
7 bytes		"Magic String"
1 byte		"Version Number"
1 byte		LZMA2 properties

File Entry [1..N entries]:
1 byte		Operation
8 bytes		Target file name length
? bytes		Target file name
8 bytes		Compressed size                   [ Added in Patch File Version 2 ]
? bytes		Compressed data
```

The Operations are defined in `PatchOperation` and can be one of the following:
    1) Delete
    2) Add
    3) Replace
    4) Add Directory

There is currently no operation for a binary delta, though it's planned for the
future.


Version History
---------------

### Version 2 and 3
Version 2 and 3 are structurally identical, except that Version 3 uses LZMA SDK
19.00, while Version 2 uses LZMA 16.02. The version bump was necessary to avoid
a potential data loss when trying to apply patches using old tools.


### Version 2
Added the `Compressed size` fieldm 8 bytes, that would make the patch file more
robust. While there was no data corruption *ever* during production use, it is
nice to have this guard in place.

This improvement also allowed the creation of tools to check the contents of a
patch file, without needing to consume its data.


### Version 1
The initial version of the patch file was feature complete, relying on the LZMA
auto detection to know where the data for a specific file ended.


### 2021-03-25 - Version 3
Upgraded LZMA SDK to version 19.00


### 2018-03-18 - Version 2
Patch files now include the compressed data size, allowing for easy navigation
of their contents. Previously, it did rely on LZMA auto detection to know when
the data was completed per file.


### 2016-03-20 - Version 1
Initial version, with full support for Windows, macOS and Linux
