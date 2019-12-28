from PIL import Image, ImageOps 
import random

# probability of single bit to flip
#noise = 0.1 / (8 * 127)
noise = -1

image_file = Image.open("input.bmp")
image = image_file.load()

binary = open("recvbytes.bin","wb")
for y in range(240):
    for x in range(320):
        byte = bytes([image[x, y]])
        
        # applying noise
        for b in range(8):
            rng = random.random()
            if rng < noise:
                sw = 0x01 << b
                new = int.from_bytes(byte, 'big') ^ sw
                byte = bytes([new])
                
                
        binary.write(byte)

binary.close()