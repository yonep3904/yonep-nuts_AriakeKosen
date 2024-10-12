def make_fig(board: list[list[int]], size: int) -> Image:
    width = len(board[0])
    height = len(board)
    img = Image.new('RGB', (width, height))
    pixels = img.load()
    for y in range(height):
        for x in range(width):
            pixels[x, y] = color_dict[board[y][x]]

    if width > height:
        new_width = size
        new_height = int(size * height / width)
        img = img.resize((new_width, new_height), resample=Image.NEAREST)
        back = Image.new('RGBA', (size, size), (0, 0, 0, 0))
        back.paste(img, (0, size//2 - new_height//2))
        return back
    elif width < height:
        new_width = int(size * width / height)
        new_height = size
        img = img.resize((new_width, new_height), resample=Image.NEAREST)
        back = Image.new('RGBA', (size, size), (0, 0, 0, 0))
        back.paste(img, (size//2 - new_width//2, 0))
        return back
    else:
        new_width = size
        new_height = size
        return img.resize((new_width, new_height), resample=Image.NEAREST)
