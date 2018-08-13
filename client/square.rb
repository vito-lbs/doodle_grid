require 'socket'
u = UDPSocket.new

IP = '10.38.0.120'
PORT = 27420
DELAY = 0.01

(0...16).each do |x|
  u.send [x, 0, 0, 0, 0].pack('ccccc'), 0, '10.38.0.120', 27420
  sleep 0.01
  u.send [x, 15, 0, 0, 0].pack('ccccc'), 0, '10.38.0.120', 27420
  sleep 0.01
  u.send [0, x, 0, 0, 0].pack('ccccc'), 0, '10.38.0.120', 27420
  sleep 0.01
  u.send [15, x, 0, 0, 0].pack('ccccc'), 0, '10.38.0.120', 27420
  sleep DELAY
  print '.'
end

(0...16).each do |x|
  u.send [x, x, 0x10, 0, 0x10].pack('ccccc'), 0, IP, PORT
end

(0...16).each do |x|

  # (0...16).each do |y|
  #   pix = i[x, y]
  u.send [x, 0, 0x01, 0x01, 0x01].pack('ccccc'), 0, '10.38.0.120', 27420
  sleep DELAY
  print '.'
end
(0...16).each do |x|

  # (0...16).each do |y|
  #   pix = i[x, y]
  u.send [0, x, 0x01, 0x01, 0x10].pack('ccccc'), 0, '10.38.0.120', 27420
  sleep DELAY
  print '.'
end
(0...16).each do |x|

  # (0...16).each do |y|
  #   pix = i[x, y]
  u.send [x, 15, 0x01, 0x20, 0x01].pack('ccccc'), 0, '10.38.0.120', 27420
  sleep DELAY
  print '.'
end
(0...16).each do |x|

  # (0...16).each do |y|
  #   pix = i[x, y]
  u.send [15, x, 0x30, 0x01, 0x01].pack('ccccc'), 0, '10.38.0.120', 27420
  sleep DELAY
  print '.'
end
