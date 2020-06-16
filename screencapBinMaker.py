import os,sys,hashlib

#this program creates the .BIN files that 3ds home menu generates when taking screenshots
#usage example: python screencapBinMaker.py HNI_0001.JPG HNI_0001.BIN
#it's unknown what these are used for and even if they're used at all

with open(sys.argv[1],"rb") as f:
	jpeg=f.read()
	
bin=hashlib.sha256(b"MG4SF5LYDDpjWviYp2gfbWJgWLbnTTVQ" + b"\x00" + jpeg)

with open(sys.argv[2],"wb") as f:
	f.write(bin.digest())