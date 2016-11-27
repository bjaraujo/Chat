import time
import socket
import select

UDP_IP = "80.79.23.184"
UDP_PORT = 1234

print "UDP target IP:", UDP_IP
print "UDP target port:", UDP_PORT

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
            
print "Start."
sock.sendto("START", (UDP_IP, UDP_PORT))
            
while True:

    print "Sending ID..."
    sock.sendto("B", (UDP_IP, UDP_PORT))

    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    
    if ":" in data: 
        break;
        
    time.sleep(2)    

sock.sendto("END", (UDP_IP, UDP_PORT))
    
UDP_IP_A = data.split(':')[0]
UDP_PORT_A = int(data.split(':')[1])
    
print "Address client A: " + UDP_IP_A + ":" + str(UDP_PORT_A)

CONNECTED = False

print "Start talking to client A"

while True:

    print "Sending message: Hello client A"
    sock.sendto("Hello client A", (UDP_IP_A, UDP_PORT_A))
    time.sleep(1)
    
'''
while True:

    sock.sendto("Hello client A", (UDP_IP_A, UDP_PORT_A))

    if not CONNECTED:
        sock.sendto("ACK", (UDP_IP, UDP_PORT))
        
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    
    if addr[0] == UDP_IP_A:
        print "--->Recieved message from client A: ", data
        CONNECTED = True
    else:
        print data

    time.sleep(4)    
'''
 