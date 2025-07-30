# MileBones
Source code for 1990's-era 16-bit Windows card game Mile Bones 2.2a
(c) 1994 André Needham

Permission is granted to use, copy, and distribute this code for educational and historical purposes only. Commercial use is strictly prohibited.

This software is provided "as is", without warranty of any kind, express or implied. The author shall not be held liable for any damages arising from its use.

---

All the code to build Mile Bones 2.2a for 16-bit Windows is here.

Rough toolchain and compile description:

Have 86Box running DOS with Windows 3.1.

Get Microsoft C 6.0a from wherever you get old software.  I got the 1.2mb install disk set from archive.org.  Make sure you get all 7 disks.  One of the uploads only had the first 6 disks.  Disk 7 says it's for OS/2, but the DOS version of c3.exe is on there, so you'll need disk 7.

Install C 6.0a.

Get Windows 3.1 Software Development Kit, again from one of the usual locations.  I chose a CD format here to try to avoid the whole floppy hassle.

Run install\install.exe.

Run c600libs\install.bat, like: install.bat c:\c600\lib c:\c600\include

This installs stuff like slibcew.lib which is needed to build any Windows app.  Why it's not part of install.exe, I don't know.

Reboot, copy all the files from this folder into a folder on your Windows 3.1 machine.

type: nmake

This should hopefully build mb.exe.

Other notes:
The code is probably also buildable under c5.1 and Windows 3.0 SDK, but I didn't try that.

The original game had context-sensitive help but I commented anything involving that out.  I had to recreate the makefile for Mile Bones from scratch since I'd lost it, and didn't want to figure out the help compiler stuff all over again.

Back in the mid-'90s I swear I had code to build this under a Win32 environment but I've apparently lost that.  I likely won't waste time re-converting it since it was kind of a pain.  Just run an ancient version of Windows in 86Box and run the game there.

Andre
