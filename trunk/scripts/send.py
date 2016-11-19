import time
import socket

UDP_IP = "80.79.23.77"
UDP_PORT = 1234
MESSAGE = "Hello, World!"

print "UDP target IP:", UDP_IP
print "UDP target port:", UDP_PORT

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP

while True:
    print "message:", MESSAGE
    sock.sendto(MESSAGE, (UDP_IP, UDP_PORT))
    time.sleep(2)
