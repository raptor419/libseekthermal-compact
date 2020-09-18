#!/usr/bin/env python3
# encoding: utf-8

import cv2
import glob

def main():
    for fn in sorted(glob.glob("*.pgm")):
        img = cv2.imread(fn,-1) # -1 for CV_LOAD_IMAGE_UNCHANGED
        size = img.shape
        center = size[1]//2
        middle = size[0]//2
        border = [
            img[1,     1], img[1,      center], img[1,      -2],
            img[middle,1], img[middle, center], img[middle, -2],
            img[-2,    1], img[-2,     center], img[-2,     -2]
        ]
        border = sum(border)//len(border)
        print(border)

if __name__ == '__main__':
    main()
