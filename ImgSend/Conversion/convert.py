from PIL import Image, ImageOps 

image_file = Image.open("input.bmp")
image = image_file.load()

header = open("image.h","w+")
header.write("static const byte bmp[] = { ")
#header.write("#define bmp { ")

for y in range(240):
    for x in range(320):
        header.write(hex(image[x, y]))
        if x != 319 or y != 239: header.write(",")
    header.write('\n')

header.write("};")
header.close()