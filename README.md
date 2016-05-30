# ZPatcher

A suite to Create and Apply patches to application and games.

## Compiling

Run the file ```.\libs\curl\projects\generate.bat``` to generate libcurl required files.
Currently, you will need to upgrade TinyXML2 project file to Visual Studio 2015.
A pull request was already made to [TinyXML2][2] to upgrade it's repository to use Visual Studio 2015.

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
```ApplyPatch.exe <Patch file> <Directory to be patched with trailing slash>```

This utility receives a zpatch file as input and a directory to be patched.
It is mandatory that the file structure is the same as the file used to create the patch.
If a file is missing - even if it's supposed to be deleted during the patching process - it will fail.

If the patch applying process fails, it reverts all the changes processed until it fails.

Missing Features of the *offline patcher*:

- There is no version tracking mechanism in place.
	
## Applications - Online Patching

### ZUpdater

It is required to setup a webserver with a XML file containing the update data.
After it's configured, you will need to edit the necessary data in ```ZUpdater/main.cpp```.
Compile the code and run it, you will see the magic happening.

It is possible to do a self-update on Windows, you just need to add an executable name with an extra "a" on it's name.
For example, to update ```ZUpdater.exe```, add a file called ```ZUpdatera.exe``` on the same directory.
This file can be delivered using a ```.zpatch``` file.

#### XML Structure

This is the structure of the required XML file for the update proccess.

```
<?xml version="1.0" ?>
<zupdater>
    <application>ZUpdater Example Application</application>
    <builds>
        <build>
            <version>223</version>
            <desc>
                <![CDATA[<p><b>ZUpdater Example Application - Revision #223</b></p>
<br/>
<ul></li>
<li>Fixed game-breaking issue #10</li>
<li>Improved user experience</li>
</ul>
]]>
</desc>
        </build>
        <build>
            <version>0</version>
            <desc>
                <![CDATA[<p><b>ZUpdater Example Application - Base</b></p>
<br/>
<ul>
<li>Initial release!</li>
</ul>
]]>
</desc>
        </build>
    </builds>
    <patches>
        <patch>
            <source_version>0</source_version>
            <destination_version>1</destination_version>
            <file>http://www.example.org/filevault/exampleapp_base.zpatch</file>
            <size>256480658</size>
            <md5>60fb5269a6b219c7431edb1f12a8315d</md5>
        </patch>
        <patch>
            <source_version>1</source_version>
            <destination_version>223</destination_version>
            <file>http://www.example.org/filevault/exampleapp_base_to_220.zpatch</file>
            <size>88780502</size>
            <md5>6fe73bcdf3bbeccfc1148bd93979038b</md5>
        </patch>
    </patches>
</zupdater>
```

## Patch Logging

There is a very simple (and extremely verbose, for now), log system in the patcher.
It will create a Logs/ folder on it's base directory and output all the operations (including failures) to it.

## Missing features

There are a number of planned, but missing features. These include, but are not limited to:

- Add hash check of the files to be updated by a patch.
- Add support for binary file deltas. Currently considering using [minibsdiff][6], but suggestions are welcome.
- Appropriate changelog display. (This will probably require a Visual Patcher).
- Patch merger (i.e. Given two incremental patch files, merge them in a single patch file).
- Update the file format so it includes the compressed block size, avoiding to use the "file ended" flag from LZMA.
- Visual patcher (i.e. "Fancy User Interface").

Please note these features are not listed in order of priority. Also, there isn't a timeframe for those features to be added.
Pull requests are welcome!

## Third Party libraries

ZPatcher uses a few external libraries to aid in it's functions:

- [libcurl][4]
- [Dirent interface for Windows][3] for directory navigation under Windows
- [LZMA SDK 16.02][1] provides the LZMA2 compression algorithm.
- [TinyXML2][2] for XML parsing
- [MD5][5] implementation by L. Peter Deutsch

CreatePatch and ApplyPatch uses ZPatcherLib, which uses [Dirent][3] and [LZMA SDK][1].

ZUpdater also uses [libcurl][4] and [TinyXML2][2].

## Final Considerations

It is my intent to make this project as platform independent as possible. As of this writing, both offline utilities are platform independent, and the online utility should follow soon.
I'd love to have a visual patcher for this project, but my current available time doesn't allow 

Contributions to the project are welcome! Get in touch if you want to contribute and be sure to send pull requests!


Last but not least, I'm not sure how to properly give credit to libraries used in this project.
If it's done in a wrong way, I'd certainly appreciate some help on how to format and give credit in an appropriate way. :)

[1]: http://www.7-zip.org/sdk.html
[2]: https://github.com/leethomason/tinyxml2
[3]: https://github.com/tronkko/dirent
[4]: https://curl.haxx.se/libcurl/
[5]: https://sourceforge.net/projects/libmd5-rfc/files/
[6]: https://github.com/thoughtpolice/minibsdiff