import numpy as np  
import cv2  
import os
root_dir = './img_s6'
files = os.listdir(root_dir)
face_cascade = cv2.CascadeClassifier("./haarcascade_frontalface_alt2.xml")

img_width = 138;
img_height = 168;

for file in files:
    path = os.path.join(root_dir,file)
    print(path)
    img = cv2.imread(path)  
    gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)         
    faces = face_cascade.detectMultiScale(gray,1.1,5,cv2.CASCADE_SCALE_IMAGE,(50,50))  
    for (x,y,w,h) in faces:
        faceROI = gray[y:y+h,x:x+w]
        MyFace = cv2.resize(faceROI, (img_width,img_height))
        cv2.imwrite(os.path.join('./s6',file), MyFace)