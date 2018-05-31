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

def calc_IOU(pred,label):
	jiaoji=0
	bingji=0
	for cols in range(pred.shape[0]):
		for rows in range(pred.shape[1]):
			if((pred[cols][rows]!=0) and (label[cols][rows]!=0)):
				jiaoji +=1
			if(((pred[cols][rows]==0) and (label[cols][rows]!=0)) or ((pred[cols][rows]!=0) and (label[cols][rows]==0))):
				bingji +=1
	IOU=jiaoji/bingji
	print(IOU)
	return IOU

with tf.Session() as sess:

	model_point = model_point.Rec_point(Network,0,1)
	total_IOU = 0
	init = tf.global_variables_initializer()
	sess.run(init)

	saver = tf.train.Saver()
	save_path = cfg.MODEL_PATH
	print('Param path:')
	print(save_path)

	saver.restore(sess, save_path)
	print('Finished reloading param.')

	[train_image, train_label, dir_] = input_data(1, cfg.LABEL_PATH, cfg.IMAGE_PATH)
	data_num = len(train_label)
	print(data_num)
	img1 = np.zeros([1,cfg.INPUT_HEIGHT,cfg.INPUT_WIDTH,cfg.IMAGE_CHANNEL])
	for k in range(int(data_num)):
		
		label = cv2.imread(train_label[k],0)
		img_origin = cv2.imread(train_image[5*k+1])
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

		time1=time.clock()
		img2_1 = sess.run(model_point.out, feed_dict={model_point.x:img_part1})
		img2_2 = sess.run(model_point.out, feed_dict={model_point.x:img_part2})
		time2=time.clock()-time1
		#print(time2,".....time")

		img3_1 = img2_1[0, 0:, 0:, 0]
		img3_2 = img2_2[0, 0:, 0:, 0]
		img3=np.hstack((img3_1,img3_2[:,cfg.TRAIN_WIDTH*2-cfg.INPUT_WIDTH:cfg.TRAIN_WIDTH]))

		kernel_point = np.ones((3,3),np.uint8)
		img_erosion = cv2.erode(img3, kernel_point, iterations = 1)
		img_dilation = cv2.dilate(img_erosion, kernel_point, iterations = 1)
		ret, img_binary = cv2.threshold(img_dilation, 100, 255, cv2.THRESH_BINARY)
		img_binary=img_binary.astype(np.uint8)

		total_IOU += calc_IOU(img_binary,label)
	print(total_IOU/int(data_num))
		



	[test_image, test_label, dir_] = input_data(1, cfg.TESTLABEL_PATH, cfg.TEST_PATH)
	data_num = len(test_label)
	print(data_num)
	img1 = np.zeros([1,cfg.INPUT_HEIGHT,cfg.INPUT_WIDTH,cfg.IMAGE_CHANNEL])
	for k in range(int(data_num)):
		
		label = cv2.imread(test_label[k],0)
		img_origin = cv2.imread(test_image[5*k+1])
		if cfg.INPUT_CHANNEL==1:
			for cc in range(cfg.IMAGE_CHANNEL):
				img = cv2.imread(test_image[5*k+cc],0)
				#cv2.imshow("aa",img)
				#cv2.waitKey()
				img = np.asarray(img)
				img = img.astype('float32')
				img = img.reshape(1,cfg.INPUT_HEIGHT,cfg.INPUT_WIDTH)
				img1[0,:,:,cc] = img

		else:
			img=cv2.imread(test_image[k])

		#img1 = img.reshape(1, cfg.INPUT_HEIGHT, cfg.INPUT_WIDTH, cfg.INPUT_CHANNEL)#src
		#src
		img_part1 = img1[:,:,:cfg.TRAIN_WIDTH,:]
		img_part2 = img1[:,:,cfg.INPUT_WIDTH-cfg.TRAIN_WIDTH:cfg.INPUT_WIDTH,:]

		time1=time.clock()
		img2_1 = sess.run(model_point.out, feed_dict={model_point.x:img_part1})
		img2_2 = sess.run(model_point.out, feed_dict={model_point.x:img_part2})
		time2=time.clock()-time1
		#print(time2,".....time")

		img3_1 = img2_1[0, 0:, 0:, 0]
		img3_2 = img2_2[0, 0:, 0:, 0]
		img3=np.hstack((img3_1,img3_2[:,cfg.TRAIN_WIDTH*2-cfg.INPUT_WIDTH:cfg.TRAIN_WIDTH]))

		kernel_point = np.ones((3,3),np.uint8)
		img_erosion = cv2.erode(img3, kernel_point, iterations = 1)
		img_dilation = cv2.dilate(img_erosion, kernel_point, iterations = 1)
		ret, img_binary = cv2.threshold(img_dilation, 100, 255, cv2.THRESH_BINARY)
		img_binary=img_binary.astype(np.uint8)

		total_IOU += calc_IOU(img_binary,label)
	print(total_IOU/int(data_num))