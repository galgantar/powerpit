from PIL import Image, ImageOps


for i in range(44):

    im = Image.open(f"{i}.png")
    out = im.transpose(Image.FLIP_TOP_BOTTOM)
    out.save(f"{i}final.png")
