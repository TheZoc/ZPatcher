Applications
============

Patch creation utilites
-----------------------

Both `CreatePatch` and `VisualCreatePatch` can be used to create the patch files.
* `CreatePatch` is better suited to create patches on batch files and scripts, being a command-line application.
* `VisualCreatePatch` is better suited to create a patch manually, since it displays a window with the patch creation process in a user friendly interface. It is also faster than the command-line counterpart.

To create a patch with `CreatePatch`, the usage is:
```
CreatePatch.exe <old version directory> <new version directory> <output patch file>
```

To create a patch with `VisualCreatePatch`, the usage is:
```
VisualCreatePatch.exe -o <old version directory> -n <new version directory> -p <output patch file>
```

The command line input is slightly different, since `VisualCreatePatch` uses wxWidgets tools to process the command line input.

Currently, both applications must be run using a command line, the `VisualCreatePatch` user interface is currently pending.
Note: You can always submit a pull request for it! Make the command line input optional and add dialogs for folder inputs.


ApplyPatch
----------

Usage:
```
ApplyPatch.exe <Patch file> <Directory to be patched with trailing slash>
```

This utility receives a zpatch file as input and a directory to be patched.
It is mandatory that the file structure is the same as the file used to create the patch.
If a file is missing - even if it's supposed to be deleted during the patching process - it will fail.

If the patch applying process fails, it reverts all the changes processed until it fails.

This application is useful to apply standalone patches. It does not have a have a version tracking mechanism.


ZUpdater
--------

Command line tool to fetch the update information from an URL, download the required patches and apply them.

It is required to setup a webserver with a XML file containing the update data.
After it's configured, you will need to edit the necessary data in `ZUpdater/main.cpp`.
Compile and run it, the magic will happen.

It is possible to do a self-update on Windows, you just need to add an executable name with an extra "a" on it's name.
For example, to update `ZUpdater.exe`, add a file called `ZUpdatera.exe` on the same directory.
This file can be delivered using a `.zpatch` file.


ZLauncher
---------

Visual application that feches the update information from an URL, download the required patches and apply them.

The setup is exactly the same as the `ZUpdater` one, but this one will also display the changes for each patch.
