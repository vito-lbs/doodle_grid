# require 'socket'
require 'chunky_png'
# u = UDPSocket.new
i = ChunkyPNG::Image.from_file '/Users/bkerley/Desktop/just-the-hash-grape-square.png'

f = File.open('hash.h', 'w')

f.puts "#pragma once"
f.puts "CRGB hash[] = {"

(0...16).each do |x|
  (0...16).each do |y|
    if 1 == (x % 2)
      y = 15 - y
    end
    pix = i[x, y]
    
    f.puts("CRGB(#{pix >> 24}, #{(pix >> 16) & 0xFF}, #{(pix >> 8) & 0xFF}),")
  end
end

f.puts "}"
