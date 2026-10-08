"""The port's icon ("two souls"): game/icon.png (256 px, window icon) and game/icon.ico (exe).

    python tools/make_icon.py          # needs Pillow; run from anywhere

Original artwork drawn here, not taken from the game or its packaging: a disc split blue / crimson
on a diagonal, a gold rim, and a longsword down the seam. Outputs are committed so building
needs no Python; rerun this after changing the drawing.
"""
import math
import os

from PIL import Image, ImageDraw, ImageFilter

S = 1024  # drawing size; outputs are downscaled from it
C = S // 2
GOLD, GOLD_DARK = (226, 182, 74, 255), (150, 108, 34, 255)
ICO_SIZES = [16, 20, 24, 32, 40, 48, 64, 96, 128, 256]


def radial(inner, outer, r):
    img = Image.new("RGBA", (S, S))
    px = img.load()
    for y in range(S):
        for x in range(S):
            t = min(1.0, math.hypot(x - C, y - C) / r)
            px[x, y] = tuple(int(inner[i] + (outer[i] - inner[i]) * t) for i in range(3)) + (255,)
    return img


def sword():
    """Vertical longsword, point down, on a transparent layer."""
    layer = Image.new("RGBA", (S, S), (0, 0, 0, 0))
    d = ImageDraw.Draw(layer)
    # blade: light left bevel, darker right bevel, a bright fuller
    d.polygon([(476, 330), (512, 330), (512, 935)], fill=(232, 238, 246, 255))
    d.polygon([(512, 330), (548, 330), (512, 935)], fill=(146, 160, 182, 255))
    d.line([(512, 340), (512, 700)], fill=(255, 255, 255, 255), width=6)
    # crossguard with flared ends and a blue stone
    d.rounded_rectangle([360, 298, 664, 338], radius=14, fill=GOLD)
    d.rectangle([372, 322, 652, 338], fill=GOLD_DARK)
    for sx in (-1, 1):
        x = C + sx * 160
        d.polygon([(x, 278), (x + sx * 30, 318), (x, 358), (x - sx * 12, 318)], fill=GOLD)
    d.ellipse([490, 296, 534, 340], fill=(40, 110, 220, 255), outline=GOLD_DARK, width=4)
    # grip with gold wraps
    d.rectangle([494, 176, 530, 298], fill=(30, 40, 96, 255))
    for y in range(190, 296, 22):
        d.line([(494, y), (530, y + 10)], fill=GOLD, width=5)
    # pommel with a red stone
    d.ellipse([480, 118, 544, 182], fill=GOLD, outline=GOLD_DARK, width=5)
    d.ellipse([500, 138, 524, 162], fill=(200, 40, 50, 255))
    return layer


def glow(layer, color, radius):
    g = Image.new("RGBA", layer.size, color)
    g.putalpha(layer.split()[3].filter(ImageFilter.GaussianBlur(radius)))
    return g


def draw():
    blue = radial((70, 170, 255), (10, 30, 90), C)
    red = radial((255, 90, 70), (80, 8, 16), C)
    split = Image.new("L", (S, S), 0)
    ImageDraw.Draw(split).polygon([(0, 0), (S, 0), (0, S)], fill=255)
    body = Image.composite(blue, red, split.filter(ImageFilter.GaussianBlur(6)))
    disc = Image.new("L", (S, S), 0)
    ImageDraw.Draw(disc).ellipse([C - 470, C - 470, C + 470, C + 470], fill=255)
    img = Image.new("RGBA", (S, S), (0, 0, 0, 0))
    img.paste(body, (0, 0), disc)
    ring = Image.new("RGBA", (S, S), (0, 0, 0, 0))
    rd = ImageDraw.Draw(ring)
    rd.ellipse([C - 488, C - 488, C + 488, C + 488], outline=(20, 16, 24, 255), width=20)
    rd.ellipse([C - 474, C - 474, C + 474, C + 474], outline=GOLD, width=12)
    img = Image.alpha_composite(img, ring)
    sw = sword()
    img = Image.alpha_composite(img, glow(sw, (255, 255, 230, 255), 22))
    img = Image.alpha_composite(img, glow(sw, (255, 255, 255, 200), 6))
    return Image.alpha_composite(img, sw)


def main():
    game = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "game")
    img = draw()
    img.resize((256, 256), Image.LANCZOS).save(os.path.join(game, "icon.png"), optimize=True)
    # Each .ico entry downscaled from the full drawing (Pillow would otherwise scale from 256).
    frames = [img.resize((n, n), Image.LANCZOS) for n in ICO_SIZES]
    frames[-1].save(os.path.join(game, "icon.ico"), sizes=[(n, n) for n in ICO_SIZES],
                    append_images=frames[:-1])
    print("wrote game/icon.png, game/icon.ico")


if __name__ == "__main__":
    main()
