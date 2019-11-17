# -*- coding: utf-8 -*-
"""
Created on Mon Jul 22 22:18:39 2019

@author: Amir.A
"""

import cv2
import os
from bitarray import bitarray
import numpy as np

## PARAMS
OUTPUT='4bits'
SIZE = 32
MIN_SIZE = 8
dirPath = './'
filename = 'bitmaps.h'
FLASH_POS = '__attribute__((section(".fini7")))' #'PROGMEM'

## PARAMS for binary image
THRESH = 127
INVERT = 'auto'

## PARAMS for rgb_4bits image
TRANSPERCY2BLACK = True
bitsP2M1 = 15 #(2^4-1)
THRESH4BITS = {'animals':250,\
               'animals2':250,\
               'cars':1,\
               'clothes':1,\
               'emoji':1,\
               'flowers':1,\
               'food':1,\
               'icons':1,\
               'music':1,\
               'numbers':1,\
               'people':250,\
               'shapes':250,\
               'weather':250}

fh = open(filename, 'w') 

for curr_dir in os.scandir(dirPath):
    if curr_dir.is_dir() and 'tmp' not in curr_dir.name:
        #print('*'*10 , curr_dir , '*'*10)
        print(curr_dir.name)
        VAR_NAME = curr_dir.name#'my_RGB_bmp'
        #imgFullPath = 'C:/Users/PC-BRO/Downloads/icons/unicorn.png'
        
        
        if OUTPUT == '4bits':
    
            ## Print variable
            header = 'static const uint16_t ' + FLASH_POS + ' \n \
                '+ VAR_NAME +'[][' + str(int(SIZE*SIZE)) + '] = {\n'
            
            strng = ''
            for curr_file in os.scandir(curr_dir.path):
                #print(curr_file.name)
                imgFullPath = curr_file.path
                ## Load image and transform to 4bits
                img = cv2.imread(imgFullPath,-1)
                img = cv2.resize(img,(SIZE,SIZE))
                if TRANSPERCY2BLACK:
                    img[np.where(img[:,:,3] < THRESH4BITS[curr_dir.name])] = 0                
                img_4_bits = (img.astype('float') / 255.0 * bitsP2M1).astype('uint8')
                
                if img_4_bits.shape[2] == 4:
                    img_4_bits = img_4_bits[:,:,:3]    
                                
                #strng+= '{\n'
                for x in range(img_4_bits.shape[0]):
                    for y in range(img_4_bits.shape[1]):
                        if x==0 and y==0:
                            strng += '{'
                        strng += '0x'
                        for c in range(img_4_bits.shape[2]):
                            strng += hex(img_4_bits[x,y,c])[2:]
                        strng += ', '
                    strng+= '\n'
                
                strng+= '},'
                
            footer = '};\n\n'
            #print(header + strng + footer)
            
        elif OUTPUT == 'bin':
            ## Load image and transform to binary
            img = cv2.imread(imgFullPath,-1)[:,:,-1]
            img = cv2.resize(img,(SIZE,SIZE))
            img_bin = np.zeros_like(img)
            img_bin[np.where(img > THRESH)] = 1
            
            ## Determine '1's and '0's
            if INVERT is 'auto':
                if img_bin.sum() > 0.5*(img_bin.shape[0]*img_bin.shape[1]):
                    INVERT = True
                else:
                    INVERT = False
            if INVERT:
                img_bin = 1 - img_bin
                
            ## Print variable
            header = 'static const uint8_t PROGMEM \n \
                '+ VAR_NAME +'[][' + str(int(SIZE*SIZE/MIN_SIZE)) + '] = {\n'
            strng = '' 
            for i in range(img_bin.shape[0]):
                #print(i)
                binStr = bitarray(img_bin[i,:].tolist()).to01()
                #strng = ''
                if i == 0:
                    strng += '{'
                for j in range(int(SIZE/MIN_SIZE)):
                    strng += 'B'
                    strng += binStr[j*MIN_SIZE:(j+1)*MIN_SIZE]
                    strng += ','
                if i == img_bin.shape[0]-1:
                    strng = strng[0:-1] + '},'
                strng += '\n'
                #print(strng)
            
            footer = '};'
            #print(header + strng + footer)
            
        #fh = open(curr_dir.name+'.txt', 'w') 
        fh.write(header + strng + footer) 
fh.close() 
# show image

#cv2.imshow('image',img)
#cv2.waitKey(0)
#cv2.destroyAllWindows()

