# coding:utf-8
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

	[train_image, train_label, dir_] = input_data(1)
	data_num = len(train_image)
	avg_loss=0
	for k in range(int(data_num)):
		if cfg.INPUT_CHANNEL==1:
			img=cv2.imread(train_image[k],0)
		else:
			img=cv2.imread(train_image[k])
		lab=cv2.imread(train_label[k])
		cv2.imshow("label",lab)
		cv2.imshow("iput",img)

		print(train_image[k])

		img1 = img.reshape(1, cfg.INPUT_HEIGHT, cfg.INPUT_WIDTH, cfg.INPUT_CHANNEL)#src
		img_gt1 = lab.reshape(1, cfg.INPUT_HEIGHT, cfg.INPUT_WIDTH, 3)
		img_gt2 = img_gt1[0, 0:, 0:, 0]
		img_gt3 = img_gt2.reshape(1, cfg.INPUT_HEIGHT, cfg.INPUT_WIDTH, 1)#label
		time1=time.clock()
		img2,loss = sess.run([model_point.out,model_point.loss], feed_dict={model_point.x:img1,model_point.y:img_gt3})
		time2=time.clock()-time1
		print(1/time2,".....time")

		print(loss,".....loss")
		avg_loss=avg_loss+loss

		img3 = img2[0, 0:, 0:, 0]
		print(img3.shape)
		#np.savetxt('n.txt',img3)
		cv2.imshow("result",img3)

		kernel_point = np.ones((3,3),np.uint8)
		img_erosion_right = cv2.erode(img3, kernel_point, iterations = 1)
		img_dilation_right = cv2.dilate(img_erosion_right, kernel_point, iterations = 1)
		cv2.namedWindow('img_e_d', 1)
		cv2.imshow('img_e_d', img_dilation_right)
		bi_thre_right, img_binary_right = cv2.threshold(img_dilation_right, 100, 255, cv2.THRESH_BINARY)
		cv2.namedWindow('img_binary_right', 1)
		cv2.imshow('img_binary_right', img_binary_right)
		img_binary_right = img_binary_right.astype('uint8')		
		_, contours_right, hierarchy_right = cv2.findContours(img_binary_right.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE) 
		cv2.drawContours(img,contours_right,-1,(0,255,255),1)  
		for c in range(len(contours_right)):
			cnt_right = contours_right[c]
			if cv2.contourArea(cnt_right) < 100:
				#print(cnt_right)
				continue
			x_right, y_right ,w_right, h_right = cv2.boundingRect(cnt_right)
			cv2.rectangle(img, (x_right,y_right), (x_right+w_right, y_right+h_right), (0,0,255), 2)
		cv2.namedWindow('right', 1)
		cv2.imshow('right', img)
		cv2.imwrite("result\\%s"%dir_[k], img)


		
		cv2.waitKey()
		
	print(avg_loss)
