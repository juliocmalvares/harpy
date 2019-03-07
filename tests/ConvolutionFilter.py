#!/usr/bin/python3
# -*-encoding:utf8-*-

from skimage import io
import numpy as np


class ConvolutionFilter(object):
    def __init__(self, img, kernel):
        self.name = img
        self.img = io.imread(img, mode='RGB')
        self.img = np.array(self.img, dtype=int)
        self.kernel = kernel
        self.peso_total = self.kernel.sum()
        print(self.peso_total)
        self.img_result = io.imread(img)

    def apply(self):
        for i in range(self.img_result.shape[0]):
            for j in range(self.img_result.shape[1]):
                self.img_result[i, j, 0] = 1
                self.img_result[i, j, 1] = 1
                self.img_result[i, j, 2] = 1
        centroit = 0
        for i in range(self.img.shape[0] - len(self.kernel)):
            for j in range(self.img.shape[1] - len(self.kernel)):
                for lin in range(len(self.kernel)):
                    for col in range(len(self.kernel)):
                        centroit += self.img[lin+i, col +
                                             j, 0] * self.kernel[lin, col]

                if self.peso_total != 0:
                    centroit = int(centroit/self.peso_total)
                if centroit <= 0:
                    centroit = 1
                elif centroit >= 255:
                    centroit = 255
                
                self.img_result.itemset((i, j, 0), centroit)
                self.img_result.itemset((i, j, 1), centroit)
                self.img_result.itemset((i, j, 2), centroit)
                # self.img_result[i, j, 0] = centroit
                # self.img_result[i, j, 1] = centroit
                # self.img_result[i, j, 2] = centroit
                centroit = 0

    def show(self):
        io.imshow(self.img_result)
        io.show()

    def img_save(self):
        io.imsave(self.name.split('.')[0] + '-conv.png', self.img_result)

    def return_img(self):
        return self.img_result


kernel = np.array([[-1, -1, -1], [-1, 8, -1], [-1, -1, -1]])
 kernel = np.array([ [-1,-1,-1], [-1, 8, -1], [-1,-1,-1] ])
 thr_filter = ConvolutionFilter('sharpen.jpg', kernel)
 thr_filter.apply()
 thr_filter.show()
 thr_filter.img_save()
