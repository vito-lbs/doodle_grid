require 'socket'
require 'chunky_png'
u = UDPSocket.new
i = ChunkyPNG::Image.from_file './diag-16x16.png'

DELAY = 0.01
BRIGHTNESS_MULTIPLIER = 1

IP = '192.168.4.1'
PORT = 27420

buf = ''

(0...16).each do |x|
  (0...16).each do |y|
    pix = i[x,y]
    r = ((pix >> 24) & 0xFF) & 0b01111
    g = ((pix >> 16) & 0xFF) & 0b01111
    b = ((pix >>  8) & 0xFF) & 0b01111
    val = (b << (4 + 4)) + (g << (4)) + (r)
    buf << [val].pack('S')
    puts "#{pix.to_s(16)} #{r.to_s(2)} #{g.to_s(2)} #{b.to_s(2)} #{val}"
  end
  print '.'
end
p buf
u.send(buf, 0, IP, PORT)
