require 'socket'
require 'chunky_png'
u = UDPSocket.new
i = ChunkyPNG::Image.from_file '/Users/bkerley/Desktop/just-the-hash-grape-square.png'

DELAY = 0.01
BRIGHTNESS_MULTIPLIER = 1

IP = '192.168.4.1'
PORT = 27420

c = 0
buf = ''

(0...16).each do |x|
  (0...16).each do |y|
    pix = i[x, y]
    buf << [x, y,
            BRIGHTNESS_MULTIPLIER * (pix >> 24),
            BRIGHTNESS_MULTIPLIER * ((pix >> 16) & 0xFF),
            BRIGHTNESS_MULTIPLIER * ((pix >> 8) & 0xFF)
           ].pack('ccccc')
    c += 1

    if buf.length > 500
      c_b = [c].pack('c')
      u.send(c_b + buf, 0, IP, PORT)
      c = 0
      buf = ''
    end
  end
  print '.'
end

c_b = [c].pack('c')
u.send(c_b + buf, 0, IP, PORT)
c = 0
buf = ''
