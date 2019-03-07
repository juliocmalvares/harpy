#!/usr/bin/python3
# coding: utf-8
from skimage import io


class GrayscaleAverageFilter(object):
    def __init__(self, img):
        self.name = img
        self.img = io.imread(img)

    def apply(self):
        for i in range(self.img.shape[0]):
            for j in range(self.img.shape[1]):
                mean = int(
                    (int(self.img[i][j][0]) + int(self.img[i][j][1]) + int(self.img[i][j][2])) / 3)
                for z in range(self.img.shape[2]):
                    self.img.itemset((i, j, z), mean)

    def show(self):
        io.imshow(self.img)
        io.show()

    def img_save(self):
        io.imsave(self.name.split('.')[0] + '-grayscaleavr.png', self.img)

    def return_img(self):
        return self.img


