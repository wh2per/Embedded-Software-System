# client3 program

import socket
import pickle
import threading
import os
import time
import picamera
from ctypes import *

clib = cdll.LoadLibrary('/home/pi/motor.so')

filename = "sending"
HOST = "192.168.137.94"
PORT = 8089
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))

s.send(pickle.dumps("pi3"))

while True:
    data = s.recv(1024)
    data = pickle.loads(data)   # wait server's signal

    if data[:2] != "11":
        print("Invalid data")
    else:
        print("Detected")

        if data[3:] == "11":
            print("Turn on flash")
            clib.pi3_turnOn()
            """
                Turn on light
                Turn on camera -> get Image -> send Image to server
            """
        else:
            print("Don't turn on flash")
            """
                Don't turn on light
                get Image -> send Image to server
            """
        # Server send i'm ready to receive image
        ack = s.recv(36)

        with picamera.PiCamera() as camera:
            camera.capture("./screenshot/sending.jpg")
        """
        testFile = "./sending.jpg"
        f = open(testFile, "rb")
        size = os.path.getsize(testFile)
        """
        f = open("./screenshot/" + filename + ".jpg", "rb")
        size = os.path.getsize("./screenshot/" + filename + ".jpg")

        print("send frame num")
        framenum = int(size / 1024) + 1
        print(framenum)
        s.send(pickle.dumps(framenum))
        print("wait ack")
        ack = s.recv(36)
        print(ack)
        for i in range(0, framenum):
            data = f.read(1024)
            s.send(data)
            ack = s.recv(36)

        print("Image send Done")
        f.close()
        clib.pi3_doorSpeakerOn()
        ack = s.recv(36)
        time.sleep(0.3)
        clib.pi3_doorOpen()
        clib.pi3_turnOff()