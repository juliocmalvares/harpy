#!/usr/bin/python3

from skimage import io
import GrayscaleAverageFilter

class ThresholdFilter(object):
    def __init__(self, img, scale):
        self.name = img
        self.img = None
        self.scale = scale
        self.gray_filter = GrayscaleAverageFilter.GrayscaleAverageFilter(self.name)
    
    def apply(self):
        self.gray_filter.apply()
        self.img = self.gray_filter.return_img()
        for i in range(self.img.shape[0]):
            for j in range(self.img.shape[1]):
                if self.img[i][j][0] > self.scale:
                    self.img.itemset((i,j,0), 255)
                else:
                    self.img.itemset((i,j,0), 0)

                if self.img[i][j][1] > self.scale:
                    self.img.itemset((i,j,1), 255)
                else:
                    self.img.itemset((i,j,1), 0)

                if self.img[i][j][2] > self.scale:
                    self.img.itemset((i,j,2), 255)
                else:
                    self.img.itemset((i,j,2), 0)
    
    def show(self):
        io.imshow(self.img)
        io.show()

    def img_save(self):
        io.imsave(self.name.split('.')[0] + '-threshold.png', self.img)
	
    def return_img(self):
        return self.img


