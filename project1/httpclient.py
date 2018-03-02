import socket
import sys
import time

def splitter(URL):
    index = URL.find("/")
    if index == -1:
        return URL,""
    IPHOST = URL[0:index]
    FILEPATH = URL[index:]
    return IPHOST,FILEPATH

def fixHTTP(URL):
    if "http://" in URL:
        URL = URL[7:]
    if "https://" in URL:
        URL = URL[8:]
    return URL

def isURL(URL):
    isURL = False
    for c in URL:
        if(c.isalpha()):
            isURL = True
    return isURL

def converter(URL,PORT):
    HOSTNAME = ""
    FILEPATH = ""
    IP = ""
    isFile = False
    URL = fixHTTP(URL)
    if(isURL(URL)):
        HOSTNAME = splitter(URL)[0]
        FILEPATH = splitter(URL)[1]
        IP = socket.gethostbyname(HOSTNAME)
    else:
        IP = splitter(URL)[0]
        FILEPATH = splitter(URL)[1]
        HOSTNAME = socket.gethostbyaddr(IP)
    if(FILEPATH==""):
        return HOSTNAME,FILEPATH,IP,PORT,isFile
    else:
        return HOSTNAME,FILEPATH,IP,PORT,True


IP = ''
HOSTNAME = ''
PORT = 5001
BUFFER = 4096
endMessage = "\r\n\r\n"
trackTime = False
FILEPATH = ""

if(len(sys.argv))==3:
    connections = converter(sys.argv[1],sys.argv[2])
    IP = connections[2]
    HOSTNAME = connections[0]
    PORT = int(connections[3])
    FILEPATH = connections[1]
    isFile = connections[4]
if (len(sys.argv)) == 4:
    connections = converter(sys.argv[2], sys.argv[3])
    IP = connections[2]
    HOSTNAME = connections[0]
    PORT =  int(connections[3])
    FILEPATH = connections[1]
    isFile = connections[4]
    trackTime = True

s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
s.settimeout(10)
s.connect((IP,PORT))
if trackTime:
    start = time.time()
    if(isFile):
        sent = "GET  " + FILEPATH + " HTTP/1.1\r\nHost: " + HOSTNAME + "\r\nConnection: close\r\n\r\n"
    else:
        sent = "GET " + "/ " + " HTTP/1.1\r\nHost: " + HOSTNAME + "\r\nConnection: close\r\n\r\n"
    s.send(sent)
    data = (s.recv(1000000000))
    end = time.time()
    print "RTT: ",(int(round((end-start) * 1000))), "milliseconds"
    print
else:
    if (isFile):
        sent = "GET " + FILEPATH + " HTTP/1.1\r\nHost: " + HOSTNAME + "\r\nConnection: close\r\n\r\n"
    else:
        sent = "GET " + "/ " +  "HTTP/1.1\r\nHost: " + HOSTNAME + "\r\nConnection: close\r\n\r\n"
    s.send(sent)
    data = (s.recv(10000000))
print(data)
s.shutdown(1)
s.close()
