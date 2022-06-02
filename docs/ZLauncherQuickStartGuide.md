🚀 ZPatcher Suite Quick start guide
====================================

You will need access to a web server. For testing purposes, you can self-host
the patch files using WAMP, XAMPP or even Python's http.server.

1. Download and extract the latest release of ZPatcher.
2. Copy the content of the `tests` folder to your webserver.
3. Navigate to `ZLauncher` folder.
4. Edit `ZLauncher.xml` and edit the `<UpdateURL>` tag to match your webserver
   address and save.
5. Run `ZLauncher` and it will download the sample patches and apply them.

This will create and then update files as listed in `tests\patches\Logs`.

You're ready to start creating your own patches!

To create a patch file, take a look at the [List of Applications][8] that can
help in this process.

[1]: docs/ZPatcherSuiteApplications.md