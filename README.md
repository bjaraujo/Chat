# Chat
A simple chat terminal that uses UDP hole punching to traverse the NAT.

## Dependencies
SDL: https://www.libsdl.org/download-1.2.php  
SDL_net: https://www.libsdl.org/projects/SDL_net/release-1.2.html  

## Compiling
Use cmake to build the project:  
https://cmake.org

## Setup
Create a server. You can use something like:  
https://www.virtualmaster.com/virtualmaster/referral/29rj1q

## Running
On the server run:
`python serverS.py`

Change `proxyIpAddress` to point to the new server.

Run host as:  
`Chat.exe --host -n Bob`

Run client as:  
`Chat.exe --client -n Alice`

A password is generated on the host. Once connected, the application drops the communication with the server and messages are encrypted using 256bit encryption. The client should enter the password to be able to decrypt messages.
