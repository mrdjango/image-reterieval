path = []

for i in range(1, 8): # images 1 to 7
    for j in range(1, 3): # rows 1 to 2
        for k in range(1, 8): # columns 1 to 7
            p = (
                "image-reterieval/test_images/"
                + str(i)
                + "/("
                + str(i + j -2)
                + ","
                + str(k)
                + ").png"
            )
            path.append(p)
print(path)
