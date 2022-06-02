💻 ZPatcher Suite Applications
===============================

List of applications
--------------------

* Patch creation tool: `CreatePatch` and `VisualCreatePatch`
* Simple patch applying tool: `ApplyPatch`
* Online patch tool (CLI): `ZUpdater`
* Launcher (GUI): `ZLauncher`


Patch creation utilites
-----------------------

To create a patch file, you will need two directories, one with the "old"
(or current) version of your application, and a second one with the "new"
version to be updated to.

Then you can pick either `CreatePatch` or `VisualCreatePatch` utilities to
create a patch file. They will create the exact same patch file, just the
interface is slightly different.

* `CreatePatch` is better suited to create patches on batch files and scripts,
  being a command-line application.
* `VisualCreatePatch` is better suited to create a patch manually, since it
  displays a window with the patch creation process in a user friendly
  interface. It is also slightly faster than the command-line counterpart.

To create a patch with `CreatePatch`, the usage is:
```
CreatePatch.exe <old version directory> <new version directory> <output patch file>
```

To create a patch with `VisualCreatePatch`, the usage is:
```
VisualCreatePatch.exe -o <old version directory> -n <new version directory> -p <output patch file>
```

The command line input is slightly different, since `VisualCreatePatch` uses
wxWidgets tools to process the command line input.

Currently, both applications must be run using a command line, the
`VisualCreatePatch` user interface is currently pending.

*Note*: You can always submit a pull request for it! Make the command line input
optional and add dialogs for folder inputs.

For example, if you decide to use the `CreatePatch` utility, considering that
the "old/current" version of the software is in the `old_version` directory, the
"new" version is in the `new_version` directory, and that you want the patch
output file to be `example.zpatch`, you can run:

```
CreatePatch ./old_version ./new_version example.zpatch
```


ApplyPatch
----------

Usage:
```
ApplyPatch.exe <Patch file> <Directory to be patched with trailing slash>
```

This utility receives a zpatch file as input and a directory to be patched.
It is mandatory that the file structure is the same as the file used to create
the patch. If a file is missing - even if it's supposed to be deleted during
the patching process - it will fail.

In case the patch applying process fails, it reverts all the changes processed
until it fails, so no data is lost in the process.

This application is useful to apply standalone patches. It does *not* have a
have a version tracking mechanism.


ZUpdater
--------

Command line tool to fetch the update information from an URL, download the
required patches and apply them.

It is required to setup a webserver with a XML file containing the update data.
After it's configured, you will need to edit the necessary data in
`ZUpdater/main.cpp`. Compile, run it, and the magic will happen.

It is possible to do a self-update on Windows, you just need to add an
executable name with an extra "a" on it's name. For example, to update
`ZUpdater.exe`, add a file called `ZUpdatera.exe` on the same directory.
This file can be delivered using a `.zpatch` file.


ZLauncher
---------

Visual application that feches the update information from an URL, download the
required patches and apply them.

The setup is exactly the same as the `ZUpdater` one, but this one will also
display the changes for each patch.


Extra Information
=================


📄 XML Structure for the Update information
--------------------------------------------

A very simple strucutre is used to keep the update information for the target
application. There is a [sample XML][1] is included in the `tests/` directory,
and it's easy to update it to your needs.


📝 Patch Logging
-----------------

There is a very simple (and extremely verbose, for now), log system in the
patcher. It will create a Logs/ folder on it's base directory and output all
the operations (including failures) to it.

Pull requests for improving the log system are welcome!


[1]: tests/zpatcher_test.xml