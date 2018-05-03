# encoding:utf-8
import numpy as np
import tensorflow as tf
import config as cfg
import time
import random

import cv2
from PIL import Image
import model_point
from tf_utils import *
import scipy
from network__ import *
import SocketMatTransmissionServer as server
import numpy
import cv2
import socket
import time
import random
from PIL import Image
import sys


with tf.Session() as sess:

	model_point = model_point.Rec_point(Network,0,1)
   
	init = tf.global_variables_initializer()
	sess.run(init)

	saver = tf.train.Saver()
	save_path = cfg.MODEL_PATH
	print('Param path:')
	print(save_path)

	saver.restore(sess, save_path)
	print('Finished reloading param.')


	#/****************************************/
	sock=server.socketConnect(55)
	print("server build")
	while True:
		sockClient,receiveImage,open=server.socketReceive(sock)
		#cv2.imshow("receiveImage",receiveImage);
		#test

		img1 = receiveImage.reshape(1, cfg.INPUT_HEIGHT, cfg.INPUT_WIDTH, cfg.INPUT_CHANNEL)#src
		img_gt1 = receiveImage.reshape(1, cfg.INPUT_HEIGHT, cfg.INPUT_WIDTH, 1)
		img_gt2 = img_gt1[0, 0:, 0:, 0]
		img_gt3 = img_gt2.reshape(1, cfg.INPUT_HEIGHT, cfg.INPUT_WIDTH, 1)#label
		time1=time.clock()
		img2,loss = sess.run([model_point.out,model_point.loss], feed_dict={model_point.x:img1,model_point.y:img_gt3})
		time2=time.clock()-time1
		print(1/time2,".....time")

		img3 = img2[0, 0:, 0:, 0]
		np.savetxt('n.txt',img3)
		#cv2.imshow("result",img3)

		kernel_point = np.ones((3,3),np.uint8)
		img_erosion_right = cv2.erode(img3, kernel_point, iterations = 1)
		img_dilation_right = cv2.dilate(img_erosion_right, kernel_point, iterations = 1)
		#cv2.namedWindow('img_e_d', 1)
		#cv2.imshow('img_e_d', img_dilation_right)
		bi_thre_right, img_binary_right = cv2.threshold(img_dilation_right, 40, 255, cv2.THRESH_BINARY)
		#cv2.namedWindow('img_binary_right', 1)
		#cv2.imshow('img_binary_right', img_binary_right)
		img_binary_right = img_binary_right.astype('uint8')		
		_, contours_right, hierarchy_right = cv2.findContours(img_binary_right.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE) 
		cv2.drawContours(receiveImage,contours_right,-1,(0,255,255),1)  
		for c in range(len(contours_right)):
			cnt_right = contours_right[c]
			if cv2.contourArea(cnt_right) < 40:
				#print(cnt_right)
				continue
			x_right, y_right ,w_right, h_right = cv2.boundingRect(cnt_right)
			cv2.rectangle(receiveImage, (x_right,y_right), (x_right+w_right, y_right+h_right), (0,0,255), 2)
		#cv2.namedWindow('right', 1)
		#cv2.imshow('right', receiveImage)


		
		#cv2.imshow('sendImage',sendImage)
		#cv2.waitKey(0);
		
		#cv2.imshow('pyserverReceive',receiveImage)
		sockClient=server.socketSend(sockClient,receiveImage)
		#cv2.waitKey(0)
		if open==1:
			server.socketDisconnect(sockClient,sock)
			sys.exit()


	#/****************************************/

