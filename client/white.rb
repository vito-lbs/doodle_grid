require 'socket'
u = UDPSocket.new

DELAY = 0.01

IP = '192.168.4.1'
PORT = 27420

c = 0
buf = ''

(0...16).each do |x|
  (0...16).each do |y|
    buf << [x, y,
            255, 255, 255
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
