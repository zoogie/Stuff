# Stuff

nds-hb-menu:
Meant as an easy way to install a twl hb environment from a cia on 3ds. Since titleDB doesn't allow cias
with srls as their content section, this app embeds bootstrap.cia in its romfs, which it installs
upon boot and copies boot.nds to sd root as well. Install from FBI titleDB under "Stuff" lol (yes I should move this
to its own repo). 

SUDOject:
Godmode9 script that installs sudokuhax to DSinternet or DSdownload play. Can uninstall them as well.
Download USA.zip from plailect's guide (magnet links) and extract game_usa.app and public_usa.sav to
sdmc:/gm9/in/SUDOject/<here>
DSdownload (00000000.app) and DSinternet (00000002.app) should be dumped to that directory upon boot assuming they aren't modified.
You may put them there manually as well.
Should work with USA, EUR, JPN, and KOR 3ds's.
Should work with clean firm which is some nice insurance.

3DS_NVRAMtool:
Dumps nvram.bin to sd card and writes it to the 3ds's spi wifi flash. Also fixes broken crc16's on the
6 wifi settings sections and DS profile 1 and 2 so you won't have to overwrite any settings, unlike NVRAM Flash Manager.

ftpd_ctrnand.cia:
Harder to find than bigfoot until now; this app does what it says on the tin: ftp's into ctrnand.
Not recommended for everyday users or morons because of the obvious brick risk.
Even if you think you're smart, only use this with ntrboot or hardmod as a backup.

screencapBinMaker.py:
This program creates the .BIN files that 3ds home menu generates when taking screenshots.

ivsdec.py
Decrypts ivs.bin to movable.sed and vice versa.
Special thanks to @luigoalma for figuring this one out!