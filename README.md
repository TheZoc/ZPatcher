[![Build Status](https://www.travis-ci.com/TheZoc/ZPatcher.svg?branch=master)](https://www.travis-ci.com/TheZoc/ZPatcher)
[![Build status](https://ci.appveyor.com/api/projects/status/i6kr70f4x40bero5/branch/master?svg=true)](https://ci.appveyor.com/project/TheZoc/zpatcher/branch/master)

🩹 ZPatcher Suite and Lib
==========================

The ZPatcher suite is a set of applications developed to Create and Apply
patches to applications and games.

The ZPatcherLib allows an experienced developer to add seamless patch applying
utilities to their application and games. The interface is intuitive and easy
to understand with the provided [Applications.md][8].


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


🚀 Quick start guide
---------------------

You will need access to a web server. For testing purposes, you can self-host
the patch files using WAMP, XAMPP or even Python's http.server.

1. Download and extract the latest release of ZPatcher.
2. Copy the content of the `tests` folder to your webserver.
3. Navigate to `ZLauncher` folder.
4. Edit `ZLauncher.xml` and edit the `<UpdateURL>` tag to match your webserver address and save.
5. Run `ZLauncher` and it will download the sample patches and apply them.

This will create and then update files as listed in `tests\patches\Logs`.

You're ready to start creating your own patches!


🗞️ Creating a patch file
-------------------------

To create a patch file, you will need two directories, one with the "old"
(or current) version of your application, and a second one with the "new"
version to be updated to.

Then you run one of the *Patch creation utilities*, as instructed in
[Applications.md][8], and it will create a new patch file for you. For example:

```
CreatePatch ./old_version ./new_version example.zpatch
```


⚙️ Compiling
-------------

The application can be compiled in Windows, macOS and Linux with a few simple
steps. Check [Compiling.md][7] for instructions on how to compile the for your
target platform.


✔️ Applications
----------------

* Patch creation tool: `CreatePatch` and `VisualCreatePatch`
* Simple patch applying tool: `ApplyPatch`
* Online patch tool (CLI): `ZUpdater`
* Launcher (GUI): `ZLauncher`

You can see more details on each application in [Applications.md][8]


📄 XML Structure for the Update information
--------------------------------------------

A very simple strucutre is used to keep the update information for the target
application. There is a [sample XML][9] is included in the `tests/` directory,
and it's easy to update it to your needs.


📝 Patch Logging
-----------------

There is a very simple (and extremely verbose, for now), log system in the
patcher. It will create a Logs/ folder on it's base directory and output all
the operations (including failures) to it.

Pull requests for improving the log system are welcome!


🖋️ Future features
-------------------

While the application is stable and mature, there is always room from
improvement. A few features are planned, but not yet implemented:

- Add hash check of the files to be updated by a patch.
- Add support for binary file deltas. Currently considering using [minibsdiff][6], but suggestions are welcome.
- Patch merger (i.e. Given two incremental patch files, merge them in a single patch file).

Please note these features are not listed in order of priority. Also, there
isn't a timeframe for those features to be added.

Pull requests are very much welcome!


📑 Third Party libraries
-------------------------

ZPatcher uses a few external libraries to aid in it's functions:

- [libcurl][1]
- [LZMA SDK 19.00][2] provides the LZMA2 compression algorithm.
- [Dirent interface for Windows][3] for directory navigation under Windows
- [RapidXML][4] for XML parsing
- [MD5][5] implementation by L. Peter Deutsch

CreatePatch and ApplyPatch uses ZPatcherLib, which uses [LZMA SDK][2] and
[Dirent][3].

ZUpdater also uses [libcurl][1] and [RapidXML][4].


📌 Final Considerations
------------------------

It is my desire to keep this project as a cross-platform lib and application.
If you attempting to compile it for a new platform and run into issues and/or
have a fix for it, please, don't hesitate to open a new Issue or Pull Request!

Contributions to the project are welcome!
Get in touch if you want to contribute and be sure to send pull requests!

Last but not least, I'm not sure how to properly give credit to libraries used
in this project. If it's done in a wrong way, I'd certainly appreciate some
help on how to format and give credit in an appropriate way. :)


[1]: https://curl.haxx.se/libcurl/
[2]: http://www.7-zip.org/sdk.html
[3]: https://github.com/tronkko/dirent
[4]: http://rapidxml.sourceforge.net/
[5]: https://sourceforge.net/projects/libmd5-rfc/files/
[6]: https://github.com/thoughtpolice/minibsdiff
[7]: docs/Compiling.md
[8]: docs/Applications.md
[9]: tests/zpatcher_test.xml
