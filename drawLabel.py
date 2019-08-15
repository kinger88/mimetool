#!/usr/bin/env python
# -*- coding: utf8 -*-


import numpy as np  
import sys,os  
import cv2
#from certifi import __main__

class KeyPoint(object):
    """docstring for KeyPoint"""
    def __init__(self):
        super(KeyPoint, self).__init__()


    def draw_point(self,img,points,color):
        # 4个点直接画
        for x, y in points:
            cv2.circle(img, (int(x), int(y)), 3, color, -1)
            #print int(y)
        return img

    def get_point(self):
        # 取出prob层结果
        points = [106.250000, 107.750000, 146.750000, 112.250000, 125.250000, 142.750000, 105.250000, 157.750000, 139.750000, 161.750000]
        result_point = []
        for i in range(1,11,2):
            result_point.append([points[i-1],points[i]]) 
        return result_point
    
def testDraw():
    kp_warp = KeyPoint()
    points =  kp_warp.get_point()
    src_img = cv2.imread('Aaron_Eckhart_0001.jpg')
    draw = src_img.copy()
    plate_p1 = (84,92) 
    plate_p2 = (161,169)
    draw = cv2.rectangle(draw,plate_p1,plate_p2,(250,0,255),2) # 画粗定位的框
    draw = kp_warp.draw_point(draw, points, (250,255,0))
    cv2.imwrite('Aaron_Eckhart_0001_res.jpg',draw)
    
def testCarBB():
    kp_warp = KeyPoint()
    src_img = cv2.imread('20190606101917353-00N.jpg')
    draw = src_img.copy()
    plate_p1 = (871,305) 
    plate_p2 = (941,332)
    draw = cv2.rectangle(draw,plate_p1,plate_p2,(250,0,255),2) # 画粗定位的框
    cv2.imwrite('20190606101917353-00N_res.jpg',draw)

def drawLabelFile(filename):
    with open(filename, 'r') as f:
        lines=f.readlines()
    kp_warp = KeyPoint()
    for line in lines:
        line=line.strip()
        components = line.split(' ')
        paths = components[0].split('\\')
        key_point = []
        for i in range(5,15,2):
            key_point.append([float(components[i]), float(components[i+1])]) 

        plate_p1 = (int(components[1]), int(components[3]))
        plate_p2 = (int(components[2]), int(components[4]))  
        savepath = 'drawImgs/'+paths[1]

        img_path = components[0].replace('\\','/')
        print img_path
        src_img = cv2.imread(img_path)
        draw = src_img.copy()
        draw = cv2.rectangle(draw,plate_p1,plate_p2,(250,0,255),2) # 画粗定位的框
        for x, y in key_point:
            cv2.circle(draw, (int(x), int(y)), 3, (250,255,0), -1)
        draw = kp_warp.draw_point(draw, key_point, (250,255,0))
        cv2.imwrite(savepath, draw)
    
def drawWiderFile(filename):
    with open(filename, 'r') as f:
        lines=f.readlines()
    kp_warp = KeyPoint()
    for line in lines:
        line=line.strip()
        components = line.split(' ')
        savepath = 'drawImgs/'+components[0]+'.jpg'
        src_img = cv2.imread(components[0]+'.jpg')
        draw = src_img.copy()

        for i in range(1,len(components),4):
            plate_p1 = (int(float(components[i])), int(float(components[i+1])))
            plate_p2 = (int(float(components[i+2])), int(float(components[i+3])))  
            draw = cv2.rectangle(draw,plate_p1,plate_p2,(250,0,255),2) # 画粗定位的框
        cv2.imwrite(savepath, draw)

    
def drawPlatesFile(filename):
    with open(filename, 'r') as f:
        lines=f.readlines()
    kp_warp = KeyPoint()
    for line in lines:
        line=line.strip()
        components = line.split(' ')
        savepath = 'drawImgs/'+components[0]
        src_img = cv2.imread(components[0])
        draw = src_img.copy()

        for i in range(1,len(components),4):
            plate_p1 = (int(float(components[i])), int(float(components[i+1])))
            plate_p2 = (int(float(components[i+2])), int(float(components[i+3])))  
            draw = cv2.rectangle(draw,plate_p1,plate_p2,(250,0,255),2) # 画粗定位的框
        cv2.imwrite(savepath, draw)

if __name__ == '__main__':
    #testDraw()
    #testCarBB()
    #drawLabelFile('labelList.txt')
    #drawWiderFile('wider_face_train_10.txt')
    drawPlatesFile('YueB_20190606112350_seg.txt')

