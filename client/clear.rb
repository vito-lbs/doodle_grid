require 'socket'
u = UDPSocket.new

DELAY = 0.01

IP = '192.168.4.1'
PORT = 27420

c = 0
buf = "\x00" * 16 * 16 * 2

u.send(buf, 0, IP, PORT)
