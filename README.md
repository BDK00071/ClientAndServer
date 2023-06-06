# Server
  - works on debian:10 (tested by container)
  - edit PORT/IPV4_ADDR in server.c if you want to change the listening IP/Port
  - the maximum length for messages is 2000, edit "szMsg" and "szBuffer" if you want to send a longer message
  - suooprts ipv4 only
  - build the program by command : **gcc server.c -o server -pthread**


# Client
  - works on debian:10 (tested by container)
  - when executing the binary, it will prompt for input of IP, port, and message, and validate their correctness
  - the maximum length for messages is 2000, edit "szBuffer" if you want to send a longer message
  - suooprts ipv4 only
  - build the program by command : **gcc client.c -o client -lm**

# Demo
  - Please get the video from the following link created by google drive:
  - https://drive.google.com/drive/folders/1F2tkRWonhny9kQDnjW_E5TPhlWYK72z5?usp=sharing
