#!/usr/bin/python3
# -*-encoding:utf8-*-

from skimage import io
from skimage.data import data_dir
import numpy as np


class ConvolutionFilterTrateBorder(object):
    def __init__(self, img, kernel):
        self.name = img
        self.img = io.imread(img, mode='RGB')
        self.kernel = kernel
        self.width = self.img.shape[0]
        self.heigth = self.img.shape[1]
        self.widthKernel = len(self.kernel)
        self.heigthKernel = len(self.kernel[1])
        self.img_result = np.zeros(shape=(self.width,self.heigth,3),dtype=np.uint8)
        self.pad = (self.heigthKernel - 1)/2 

    def apply(self):
       
        
        for i in range(self.width  -  self.widthKernel):
            for j in range(self.heigth -  self.heigthKernel):

                sumR = 0
                sumG = 0
                sumB = 0
                div = 0  

                for x in range(self.widthKernel):
                    for y in range(self.heigthKernel):

                        Xpos = i + x - self.pad
                        Ypos =  y + j - self.pad

                        if(Xpos >= 0 and Xpos < self.width  and Ypos >=0 and Ypos < self.heigth):

                                    r = int(self.img[i+x,y+j,0])
                                    g = int(self.img[i+x,y+j,1])
                                    b = int(self.img[i+x,y+j,2])

                                    sumR += r * self.kernel[x][y]
                                    sumG += g * self.kernel[x][y]
                                    sumB += b * self.kernel[x][y]
                                    div += self.kernel[x][y]
                            


                if(div == 0): div = 1
                sumR = int( sumR/div)
                sumG = int(sumG / div)
                sumB = int(sumB /div)

                sumR = 0 if  sumR  < 0 else  sumR
                sumR = 255 if  sumR > 255 else  sumR

                sumG  = 0 if sumG  < 0 else sumG 
                sumG  = 255 if sumG  > 255 else sumG 

                sumB = 0 if sumB < 0 else sumB
                sumB = 255 if sumB > 255 else sumB
                
                self.img_result.itemset((i, j, 0), sumR)
                self.img_result.itemset((i, j, 1), sumG)
                self.img_result.itemset((i, j, 2), sumB)
                

    def show(self):
        io.imshow(self.img_result)
        io.show()

    def img_save(self):
        io.imsave(self.name.split('.')[0] + '-conv.png', self.img_result)

    def return_img(self):
        return self.img_result


kernel = np.array([[-1, -1, -1], [-1, 8, -1], [-1, -1, -1]])
kernel = np.array([ [-1,-1,-1], [-1, 8, -1], [-1,-1,-1] ])
thr_filter = ConvolutionFilterTrateBorder('sharpen.jpg', kernel)
thr_filter.apply()
thr_filter.show()
thr_filter.img_save()

