import time
import socket

UDP_IP = ""
UDP_PORT = 1234

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))

UDP_IP_A = ""
UDP_PORT_A = 0

UDP_IP_B = ""
UDP_PORT_B = 0

A_CONNECTED = False
B_CONNECTED = False

while True:
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes

    print "Received message from client:", data
        
    if data == "A":
        UDP_IP_A = addr[0];
        UDP_PORT_A = addr[1];

        sock.sendto("ACK", (UDP_IP_A, UDP_PORT_A))
        
        if UDP_IP_B != "": 
            sock.sendto(UDP_IP_B + ":" + str(UDP_PORT_B), (UDP_IP_A, UDP_PORT_A))
            sentToA = True
            
    elif data == "B":
        UDP_IP_B = addr[0];
        UDP_PORT_B = addr[1];

        sock.sendto("ACK", (UDP_IP_B, UDP_PORT_B))
        
        if UDP_IP_A != "": 
            sock.sendto(UDP_IP_A + ":" + str(UDP_PORT_A), (UDP_IP_B, UDP_PORT_B))
            sentToB = True
         
    elif data == "START":
        UDP_IP_A = ""
        UDP_IP_B = ""

        A_CONNECTED = False
        B_CONNECTED = False
        
    elif data == "END":
        
        if addr[0] == UDP_IP_A:
            A_CONNECTED = True

        if addr[0] == UDP_IP_B:
            B_CONNECTED = True
            
    if A_CONNECTED and B_CONNECTED:
                
        sock.sendto("ACK", (UDP_IP_A, UDP_PORT_A))
        sock.sendto("ACK", (UDP_IP_B, UDP_PORT_B))

    time.sleep(1)

