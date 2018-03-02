import socket
import sys
from threading import Thread
import time

TMDG = file.read(open("TMDG.html"))

PORT = int(sys.argv[1])
IP = socket.gethostbyname(socket.gethostname())

def singleThread(cSocket,cAdr):
    while 1:
        try:#Was getting errors without these try blocks but everything worked fine not sure what that was but the try fixes it
            request = cSocket.recv(2048)
        except:
            break
        response = " "
        if "GET / HTTP/1.1" in request or "GET /TMDG.html HTTP/1.1" in request or "GET TMDG.html HTTP/1.1":
            response = "HTTP/1.1 200 OK \r\n"
            response = response + "Content-type: text/html\r\nConnection: Closed \r\n \r\n" + TMDG
        else:
            response = "HTTP/1.1 404 Not Found \r\n\r\n"
        if "stop" in request:
            sys.exit()
        try:#Same as above, it was bad file descriptor error
            cSocket.send(response)
        except:
            break
        cSocket.close()


server = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
server.bind((IP,PORT))
server.listen(10)
print "HTTP Server launched, connect with IP: " + IP + " and Port: " + str(PORT)
while (True):
    connection,address = server.accept()
    Thread(target=singleThread,args=(connection,address)).start()
    input = raw_input("Enter stop to terminate server")
    if "stop" in input:
        break
server.shutdown(1)
server.close()
print("Server has terminated properly")
