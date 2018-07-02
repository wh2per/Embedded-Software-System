# server program
import socket
import pickle
import datetime
import time
import threading

"""
#1 accept 2 pi
#2 receive data from pi2 about light, detection
#3 make command [detection signal | flash on/off]
#4 wait picture from pi3
#5 save picture
"""


class ControlServer(object):
    LIGHT_LIMIT = 0  # light limit

    def __init__(self):
        self.HOST = ''
        self.PORT = 8089
        self.piList = {}

        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.bind((self.HOST, self.PORT))
        self.s.listen(1)

        for i in range(0, 2):
            conn, addr = self.s.accept()
            print('Connected by', addr)

            time.sleep(0.3)

            pitype = conn.recv(1024)
            pitype = pickle.loads(pitype)
            print("Type ", pitype, "Connected.")

            if pitype == "pi2":
                self.piList["pi2"] = conn
            else:
                self.piList["pi3"] = conn

        self.processing()
    """
    PROTOCOL
    [2 byte detect flag | 2 byte light flag]
    byte size of original data is 4 byte
    pickle.dumps packet size is 4 + 10 byte
    """

    def processing(self):
        try:
            while True:
                # receive detection data with certain interval
                detection = self.piList["pi2"].recv(1024)
                detection = pickle.loads(detection)
                print(detection.__len__())
                if detection.__len__() > 5 :
                    print("Invalid Data")
                    self.piList["pi2"].send(pickle.dumps("NCK"))
                    continue
                print(detection[:2] + "-" + detection[3:])

                # Analyze packet
                if detection[:2] == "00":
                    currenttime = datetime.datetime.now()
                    print(currenttime.strftime('%Y-%m-%d %T'), " - No detection.")
                    self.piList["pi2"].send(pickle.dumps("ACK"))
                elif detection[:2] == "11":
                    print("Detected")
                    currenttime = datetime.datetime.now()
                    print(currenttime.strftime('%Y-%m-%d %T'), " - Detection.")

                    lightFlag = detection[3:]
                    if lightFlag == "11":
                        print("It's dark now. Turn on LED")
                        self.piList["pi3"].send(pickle.dumps("11,11"))
                    else:
                        print("It's bright now. Turn off LED")
                        self.piList["pi3"].send(pickle.dumps("11,00"))
                    # Receive capture image
                    receiveImage(self.piList["pi3"], currenttime.strftime('%Y-%m-%d_%T'), lightFlag)
                    input()
                    self.piList["pi3"].send(pickle.dumps("ACK"))
                    self.piList["pi2"].send(pickle.dumps("ACK"))
                else:
                    print("Invalid Data")
                    self.piList["pi2"].send(pickle.dumps("NCK"))
        except socket.error:
            self.piList["pi2"].close()
            self.piList["pi3"].close()
        except KeyError:
            print("pi3 is Not connected")
            pass

# Receive and Save image
"""
#0 pi3 receive server's detection & light signal
#1 pi3 wait server's ACK
#2 pi3 send packet number
#3 Start image transfer
"""


def receiveImage(s, currtime, lightflag):
    print("Start image transfer")
    f = open("./capture/" + currtime + "_" + lightflag + ".jpg", "wb")
    #f = open("test.jpg", "wb")

    s.send(pickle.dumps("ACK"))

    framenum = s.recv(1024)
    print(framenum)
    framenum = pickle.loads(framenum)
    print("Num of packet - ", framenum)
    s.send(pickle.dumps("ACK"))

    for i in range(0, framenum):
        data = s.recv(1024)
        print(data.__len__())
        f.write(data)
        s.send(pickle.dumps("ACK"))
    print("Done")
    f.close()


if __name__ == "__main__":
    server = ControlServer()