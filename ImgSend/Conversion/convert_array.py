from PIL import Image, ImageOps 

image_file = Image.open("input.bmp")
image = image_file.load()

header = open("image.h","w+")
header.write("static const byte bmp[240][320] = {\n")

for y in range(240):
    header.write("{")
    for x in range(320):
        header.write(hex(image[x, y]))
        if x != 319: header.write(",")
    header.write("}")
    if y != 239: header.write(",")
    header.write("\n")
header.write("};")
header.close()