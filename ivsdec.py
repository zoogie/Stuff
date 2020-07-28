from Cryptodome.Cipher import AES
from Cryptodome.Hash import CMAC
import os,sys,hashlib,struct,time

nkey_0x35ivs=0xd772eb0e239cfab8bb73dd4a8d8edc34   #keyY from p9 rodata: 0xa717802dea9776137ba16cd389141cf0
nkey_0x35mac=0xfc19ff860778762aea8de4c1f284e1ae   #keyY from p9 rodata: 0x6E1E6BCB9EE098DD67AE771AD73B2CC9

def int16bytes(n):
	return struct.pack(">QQ",n>>64,n & ((1<<64)-1))
	
def getCMAC(msed):
	hash=hashlib.sha256(msed[:0x110]).digest()
	key = int16bytes(nkey_0x35mac)
	cipher = CMAC.new(key, ciphermod=AES)
	result = cipher.update(hash)
	return result.digest()

def getIVS():
	with open("movable.sed","rb") as f:
		msed=f.read(0x120)
	CMAC=getCMAC(msed)
	cipher = AES.new(int16bytes(nkey_0x35ivs), AES.MODE_CBC, CMAC)
	ivs=CMAC+cipher.encrypt(msed)
	with open("ivs.bin","wb") as f:
		f.write(ivs)
		
def getMSED():
	with open("ivs.bin","rb") as f:
		ivs=f.read()
	CMAC=ivs[:0x10]
	cipher = AES.new(int16bytes(nkey_0x35ivs), AES.MODE_CBC, CMAC)
	msed=cipher.decrypt(ivs[0x10:])
	if CMAC==getCMAC(msed):
		print("CMAC good")
	else:
		print("CMAC bad")
	with open("movable.sed","wb") as f:
		f.write(msed)

choice=input("Please enter choice:\n  1) Encrypt movable.sed -> ivs.bin\n  2) Decrypt ivs.bin -> movable.sed\n\n>>> ")

if int(choice) == 1:
	getIVS()
	print("Encryption complete")
elif int(choice) == 2:
	getMSED()
	print("Decryption complete")
else: print("Error: unknown input")
time.sleep(2)