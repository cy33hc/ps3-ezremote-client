# ezRemote Client

ezRemote Client is an application that allows you to connect the PS3 to remote FTP, SMB(Windows Share), NFS, WebDAV, HTTP servers to transfer files. The interface is inspired by Filezilla client which provides a commander like GUI.

![Preview](/screenshot.jpg)

## Features
 - Transfer files back and forth between PS3 and FTP/SMB/NFS/WebDAV/HTTP server
   - Support for connecting to Http Servers like (Apache/Nginx,Microsoft IIS, NPX Serve) with html directory listings. 
 - Install PKG files from any remote Server. This will automatically download the PKG to the PS3 and install the rap/rif files if any found. Creates the install bubble in the 
   - The rap/rif files must be in the same folder or in a exdata sub folder where the PKG file exists on the remote server. If a rif file already exists in the PS3 account, the rif file won't be replaced.
   - In the application settings dialog, you have the option of chosing the PS3 account for installation of the rap/rif files
 - Create Zip files
 - Extract from zip and 7zip files
 - File management function include cut/copy/paste/rename/delete/new folder/file for files on PS3.
 - Simple Text Editor to make simply changes to config text files. Limited to edit files over 32kb and limited to edit lines up to 512 characters. If you try edit lines longer then 512 characters, it will be truncated. For common text files with the following extensions (txt, log, ini, json, xml, html, conf, config) selecting them in the file browser with the X button will automatically open the Text Editor.

## Usage
To distinguish between FTP, SMB, NFS, WebDAV or HTTP, the URL must be prefix with **ftp://**, **smb://**, **nfs://**, **webdav://**, **webdavs://**, **http://** and **https://**

 - The url format for FTP is
   ```
   ftp://hostname[:port]

     - hostname can be the textual hostname or an IP address. hostname is required
     - port is optional and defaults to 21(ftp) if not provided
   ```

 - The url format for SMB(Windows Share) is
   ```
   smb://hostname[:port]/sharename

     - hostname can be the textual hostname or an IP address. hostname is required
     - port is optional and defaults to 445 if not provided
     - sharename is required
   ```

 - The url format for NFS is
   ```
   nfs://hostname[:port]/export_path[?uid=<UID>&gid=<GID>]

     - hostname can be the textual hostname or an IP address. hostname is required
     - port is optional and defaults to 2049 if not provided
     - export_path is required
     - uid is the UID value to use when talking to the server. Defaults to 65534 if not specified.
     - gid is the GID value to use when talking to the server. Defaults to 65534 if not specified.

     Special characters in 'path' are escaped using %-hex-hex syntax.

     For example '?' must be escaped if it occurs in a path as '?' is also used to
     separate the path from the optional list of url arguments.

     Example:
     nfs://192.168.0.1/my?path?uid=1000&gid=1000
     must be escaped as
     nfs://192.168.0.1/my%3Fpath?uid=1000&gid=1000
   ```

 - The url format for WebDAV is
   ```
   webdav://hostname[:port]/[url_path]
   webdavs://hostname[:port]/[url_path]

     - hostname can be the textual hostname or an IP address. hostname is required
     - port is optional and defaults to 80(webdav) and 443(webdavs) if not provided
     - url_path is optional based on your WebDAV hosting requiremets
   ```

- The url format for HTTP Server is
   ```
   http://hostname[:port]/[url_path]
   https://hostname[:port]/[url_path]
     - hostname can be the textual hostname or an IP address. hostname is required
     - port is optional and defaults to 80(http) and 443(https) if not provided
     - url_path is optional based on your HTTP Server hosting requiremets
   ```
Tested with following WebDAV server:
 - **(Recommeded)** [Rclone](https://rclone.org/) - For hosting your own WebDAV or HTTP server. (Recommended since this allow you to setup rclone as a proxy for many public file share websites like mega, google drive, onedrive etc.. https://rclone.org/#providers)
 - [Dufs](https://github.com/sigoden/dufs) - For hosting your own WebDAV server.
 - [SFTPgo](https://github.com/drakkan/sftpgo) - For local hosted WebDAV server. Can also be used as a webdav frontend for Cloud Storage like AWS S3, Azure Blob or Google Storage.
 - box.com (Note: delete folder does not work. This is an issue with box.com and not the app)
 - mega.nz (via the [megacmd tool](https://mega.io/cmd))
 - 4shared.com
 - drivehq.com

## Installation
Just install the **ps3-ezremote-client.pkg**. Use application settings dialog for changing config.

## Controls
```
Triangle - Menu (after a file(s)/folder(s) is selected)
Cross - Select Button/TextBox
Circle - Un-Select the file list to navigate to other widgets or Close Dialog window in most cases
Square - Mark file(s)/folder(s) for Delete/Rename/Upload/Download
R1 - Navigate to the Remote list of files
L1 - Navigate to the Local list of files
Start Button - Exit Application
```

## Multi Language Support
The appplication support following languages.

**Note:** Due to new strings added, there are about 31 missing translations for all the languagess. Please help by downloading this [Template](https://github.com/cy33hc/ps3-ezremote-client/blob/master/pkgfiles/USRDIR/langs/English.ini), make your changes and submit an issue with the file attached for the language.

The following languages are auto detected.
```
Dutch
English
French
German
Italiano
Japanese
Korean
Polish
Portuguese_BR
Russian
Spanish
Simplified Chinese
Traditional Chinese
```

The following aren't standard languages supported by the PS3, therefore requires a config file update.
```
Arabic
Catalan
Croatian
Euskera
Galego
Greek
Hungarian
Indonesian
Romanian
Ryukyuan
Thai
Turkish
Ukrainian
```

**HELP:** There are no language translations for the following languages, therefore not support yet. Please help expand the list by submitting translation for the following languages. If you would like to help, please download this [Template](https://github.com/cy33hc/ps3-ezremote-client/blob/master/pkgfiles/USRDIR/langs/English.ini), make your changes and submit an issue with the file attached.
```
Finnish
Swedish
Danish
Norwegian
Czech
Vietnamese
```
or any other language that you have a traslation for.
