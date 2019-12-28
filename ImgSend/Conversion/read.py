from PIL import Image

image = open("recvbytes.bin", "rb")
file = image.read()
image.close()

barray = bytes(file)

outimg = Image.frombytes('L', (320, 240), barray)
outimg.save("output.bmp", "BMP")