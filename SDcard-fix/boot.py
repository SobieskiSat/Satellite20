import os
import pyb
import machine

machine.Pin('A8', mode=machine.Pin.OUT, pull=None)

sd = pyb.SDCard()
os.mount(sd, '/sd')