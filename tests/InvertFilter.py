#!/usr/bin/python3

from skimage import io

class InvertFilter(object):
    def __init__(self, img):
        self.name = img
        self.img = io.imread(img)
    
    def apply(self):
        for i in range(self.img.shape[0]):
            for j in range(self.img.shape[1]):
                self.img.itemset((i,j,0), (255-self.img[i][j][0]))
                self.img.itemset((i,j,1), (255-self.img[i][j][1]))
                self.img.itemset((i,j,2), (255-self.img[i][j][2]))
    
    def show(self):
        io.imshow(self.img)
        io.show()

    def img_save(self):
        io.imsave(self.name.split('.')[0] + '-invertfilter.png', self.img)
	
    def return_img(self):
        return self.img


