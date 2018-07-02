# client1 program

import socket
import pickle
import threading
import os
import time
from ctypes import *

clib = cdll.LoadLibrary('/home/pi/ultrasonic.so')
clib2 = cdll.LoadLibrary('/home/pi/light_lib.so')
isdetected = False
lightInfo = False
filename = ""
HOST = "192.168.137.94"
PORT = 8089
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))

s.send(pickle.dumps("pi2"))

time.sleep(2)
while True:
    """
    Get detection data [is detected, light info]
    """

    dist = clib.get_ultrasonic_distance()
    print(dist)

    if dist > 0 and dist < 18:
        isdetected = True
        lightInfo = clib2.get_value()
    else:
        isdetected = False

    if isdetected:
        if lightInfo:
            s.send(pickle.dumps("11,11"))
        else:
            s.send(pickle.dumps("11,00"))
    else:
        print("no detection")
        s.send(pickle.dumps("00,00"))
    ack = s.recv(36)
    time.sleep(3)

