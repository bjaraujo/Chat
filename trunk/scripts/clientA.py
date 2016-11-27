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
    sock.sendto("A", (UDP_IP, UDP_PORT))
    
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    
    if ":" in data: 
        break;
        
    time.sleep(2)

sock.sendto("END", (UDP_IP, UDP_PORT))
    
UDP_IP_B = data.split(':')[0]
UDP_PORT_B = int(data.split(':')[1])
    
print "Address client B: " + UDP_IP_B + ":" + str(UDP_PORT_B)

CONNECTED = False

for i in range(1, 5):
    print "Sending message: Hello client B"
    sock.sendto("Hello client B", (UDP_IP_B, UDP_PORT_B))    
    time.sleep(1)

print "Listening for client B"
while True:

    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes

    if addr[0] == UDP_IP_B:
        print "--->Recieved message from client B: ", data
        sock.sendto("Hello client B", (UDP_IP_B, UDP_PORT_B))

    time.sleep(1)

'''    
while True:

    sock.sendto("Hello client B", (UDP_IP_B, UDP_PORT_B))

    if not CONNECTED:
        sock.sendto("ACK", (UDP_IP, UDP_PORT))

    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    
    if addr[0] == UDP_IP_B:
        print "--->Recieved message from client B: ", data
        CONNECTED = True
    else:
        print data
    
    time.sleep(4)    
'''

