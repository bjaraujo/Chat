# Chat
A simple chat terminal that uses UDP hole punching to traverse the NAT.

## Dependencies
SDL: https://www.libsdl.org/download-1.2.php
SDLnet: https://www.libsdl.org/projects/SDL_net/release-1.2.html

## Compiling
Use cmake to build
https://cmake.org

## Setup
Run serverS.py on a public server. You can use something like:
https://www.virtualmaster.com

Run host as:
Chat.exe --host -n Bob

Run client as:
Chat.exe --client -n Alice

