# encoding:utf-8
import numpy as np
import tensorflow as tf
import config as cfg
import time
import random
import os
os.environ["CUDA_VISIBLE_DEVICES"] = cfg.GPU_NUMBER

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
	data_num = len(train_label)
	img1 = np.zeros([1,cfg.INPUT_HEIGHT,cfg.INPUT_WIDTH,cfg.IMAGE_CHANNEL])
	for k in range(int(data_num)):
		

		img_origin = cv2.imread(train_image[5*k+1])
		print(img_origin.shape)
		if cfg.INPUT_CHANNEL==1:
			for cc in range(cfg.IMAGE_CHANNEL):
				img = cv2.imread(train_image[5*k+cc],0)
				#cv2.imshow("aa",img)
				#cv2.waitKey()
				img = np.asarray(img)
				img = img.astype('float32')
				img = img.reshape(1,cfg.INPUT_HEIGHT,cfg.INPUT_WIDTH)
				img1[0,:,:,cc] = img

		else:
			img=cv2.imread(train_image[k])

		#img1 = img.reshape(1, cfg.INPUT_HEIGHT, cfg.INPUT_WIDTH, cfg.INPUT_CHANNEL)#src
		#src
		img_part1 = img1[:,:,:cfg.TRAIN_WIDTH,:]
		img_part2 = img1[:,:,cfg.INPUT_WIDTH-cfg.TRAIN_WIDTH:cfg.INPUT_WIDTH,:]
		print(img_part2.shape)

		time1=time.clock()
		#img2_1 = sess.run(model_point.out, feed_dict={model_point.x:img_part1})
		#img2_2 = sess.run(model_point.out, feed_dict={model_point.x:img_part2})
		img2_1,img2_2 = sess.run([model_point.out,model_point.out],feed_dict={model_point.x:img_part1,model_point.x:img_part2})
		time2=time.clock()-time1
		print(time2,".....time")

		img3_1 = img2_1[0, 0:, 0:, 0]
		img3_2 = img2_2[0, 0:, 0:, 0]
		print(img3_1.shape)
		print(img3_2.shape)
		img3=np.hstack((img3_1,img3_2[:,cfg.TRAIN_WIDTH*2-cfg.INPUT_WIDTH:cfg.TRAIN_WIDTH]))
		print(img3.shape)

		kernel_point = np.ones((3,3),np.uint8)
		img_erosion_right = cv2.erode(img3, kernel_point, iterations = 1)
		img_dilation_right = cv2.dilate(img_erosion_right, kernel_point, iterations = 1)
		#cv2.namedWindow('img_e_d', 1)
		#cv2.imshow('img_e_d', img_dilation_right)
		bi_thre_right, img_binary_right = cv2.threshold(img_dilation_right, 70, 255, cv2.THRESH_BINARY)
		#cv2.imshow("66666666",img_binary_right)
		img_binary_right = img_binary_right.astype('uint8')
		_, contours_right, hierarchy_right = cv2.findContours(img_binary_right.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
		img666 = img_origin
		contours_final =[]
		for c in range(len(contours_right)):
			cnt_right = contours_right[c]
			if cv2.contourArea(cnt_right) < 150:
				#print(cnt_right)
				continue
			contours_final.append(cnt_right)
			x_right, y_right ,w_right, h_right = cv2.boundingRect(cnt_right)
			cv2.rectangle(img666, (x_right,y_right), (x_right+w_right, y_right+h_right), (255,0,0), 2)
		cv2.namedWindow('right', 0)
		cv2.imshow('right', img666)
		cv2.waitKey()
		