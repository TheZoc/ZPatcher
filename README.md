[![Build Status](https://www.travis-ci.com/TheZoc/ZPatcher.svg?branch=master)](https://www.travis-ci.com/TheZoc/ZPatcher)
[![Build status](https://ci.appveyor.com/api/projects/status/i6kr70f4x40bero5/branch/master?svg=true)](https://ci.appveyor.com/project/TheZoc/zpatcher/branch/master)

ZPatcher
========

The ZPatcher suite is a set of applications developed to Create and Apply
patches to both applicatoin and games.

![ZLauncher Sample Screenshot](images/ZLauncher.png)

Features:
* Multiplatform, currently supporting Windows, OSX and Linux.
* Open Source
* Easy to Create patch files for your game/application
* Skinnable
* Command line tools available to be easily included in deployment scripts
* Client-Side automatic Download-and-Apply patching process
* File backup when applying a patch, reducing the chances of a broken build on clients
* (Launcher Only) Display the Change Log for your patches


Quick start guide
-----------------

You will need access to a web server.
For testing purposes, you can use WAMP, XAMPP or even Python's http.server.

1. Download and extract the latest release of ZPatcher.
2. Copy the content of the `tests` folder to your webserver.
3. Navigate to `ZLauncher` folder.
4. Edit `ZLauncher.xml` and edit the `<UpdateURL>` tag to match your webserver address and save.
5. Run `ZLauncher.exe` and it will download the sample patches and apply them.

This will create and then update files as listed in `tests\patches\Logs`.

You're ready to start creating patches!


Compiling
---------

The application can be compiled with a few simple steps.
Check [Compiling.md][7] for instructions on how to compile the application suite.


Applications
------------

* Patch creation tool: `CreatePatch` and `VisualCreatePatch`
* Simple patch applying tool: `ApplyPatch`
* Online patch tool (CLI): `ZUpdater`
* Launcher (GUI): `ZLauncher`

You can see more details on each application in [Applications.md][8]


XML Structure for the Update information
----------------------------------------

A [sample XML][9] is included in the `tests/` directory. You can see the structure of it there.


Patch Logging
-------------

There is a very simple (and extremely verbose, for now), log system in the patcher.
It will create a Logs/ folder on it's base directory and output all the operations (including failures) to it.
Pull requests for improving the log system are welcome!


Missing features
----------------

There are a number of planned, but missing features. These include, but are not limited to:

- Add hash check of the files to be updated by a patch.
- Add support for binary file deltas. Currently considering using [minibsdiff][6], but suggestions are welcome.
- Patch merger (i.e. Given two incremental patch files, merge them in a single patch file).
- Update the file format so it includes the compressed block size, avoiding to use the "file ended" flag from LZMA.

Please note these features are not listed in order of priority. Also, there isn't a timeframe for those features to be added.
Pull requests are welcome!


Third Party libraries
---------------------

ZPatcher uses a few external libraries to aid in it's functions:

- [libcurl][4]
- [Dirent interface for Windows][3] for directory navigation under Windows
- [LZMA SDK 19.00][1] provides the LZMA2 compression algorithm.
- [RapidXML][2] for XML parsing
- [MD5][5] implementation by L. Peter Deutsch

CreatePatch and ApplyPatch uses ZPatcherLib, which uses [Dirent][3] and [LZMA SDK][1].

ZUpdater also uses [libcurl][4] and [RapidXML][2].


Final Considerations
--------------------

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
[7]: docs/Compiling.md
[8]: docs/Applications.md
[9]: tests/zpatcher_test.xml
