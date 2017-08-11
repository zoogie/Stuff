make clean
make
bannertool makebanner -i "cia/banner.png" -a "cia/audio.wav" -o "cia/banner.bnr"
bannertool makesmdh -i "cia/icon.png" -l "built with devkitARM" -s "nds-hb-menu bootstrap" -p "http://devkitpro.org" -o "cia/icon.icn"
makerom -f cia -o nds-hb-menu.cia -rsf cia/template.rsf -target t -elf nds-hb-menu.elf -icon cia/icon.icn -banner cia/banner.bnr -exefslogo
cp *.cia i:/CIA/


pause