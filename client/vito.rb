require 'socket'
require 'chunky_png'
u = UDPSocket.new
i = ChunkyPNG::Image.from_file './vito-16x16.png'

DELAY = 0.01
BRIGHTNESS_MULTIPLIER = 1

IP = '192.168.4.1'
PORT = 27420

c = 0
buf = ''

(0...16).each do |x|
  (0...16).each do |y|
    pix = i[x,y]
    r = ((pix >> 24) & 0xFF) & 0b011111
    g = ((pix >> 16) & 0xFF) & 0b011111
    b = ((pix >>  8) & 0xFF) & 0b011111
    val = (b << (5 + 5)) + (g << (5)) + (r)
    buf << [val].pack('S')
    c += 1
  end
  print '.'
end

u.send(buf, 0, IP, PORT)
