#!/usr/bin/env python
import time, socket, struct
from math import sin

# This is dirty python script to feed data for tests :
# 7.1 is fed with sinusoid
# 0.2 is fed with square

DT = 0.1
CONFIG_IP = 'localhost'
CONFIG_PORT = 8000

t = 0
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind((CONFIG_IP, CONFIG_PORT))
s.listen(1)

conn, addr = s.accept()
print('Connected by', addr)

while True:
	msg = b'\x07\x00\x01\x00' + struct.pack('<f', sin(t))
	msg += b'\x00\x00\x02\x00' + struct.pack('<l', int(t)%2)
	t += DT
	conn.send(msg)
	time.sleep(DT)

conn.close()
