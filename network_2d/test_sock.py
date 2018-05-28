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
	sock=server.socketConnect(66)
	print("2D server build")
	while True:
		img1 = np.zeros([1,cfg.INPUT_HEIGHT,cfg.INPUT_WIDTH,cfg.IMAGE_CHANNEL])
		for cc in range(cfg.IMAGE_CHANNEL)
			sockClient,receiveImage,open=server.socketReceive(sock)
			#cv2.imshow("receiveImage",receiveImage);
			#cv2.waitKey()
			receiveImage = np.asarray(receiveImage)
			receiveImage = receiveImage.astype('float32')
			receiveImage = receiveImage.reshape(1,cfg.INPUT_HEIGHT,cfg.INPUT_WIDTH)
			img1[0,:,:,cc] = receiveImage


		img_part1 = img1[:,:,:cfg.TRAIN_WIDTH,:]
		img_part2 = img1[:,:,cfg.INPUT_WIDTH-cfg.TRAIN_WIDTH:cfg.INPUT_WIDTH,:]
		print(img_part2.shape)

		time1=time.clock()
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
		img_erosion = cv2.erode(img3, kernel_point, iterations = 1)
		img_dilation = cv2.dilate(img_erosion, kernel_point, iterations = 1)
		ret, img_binary = cv2.threshold(img_dilation, 100, 255, cv2.THRESH_BINARY)
		img_binary=img_binary.astype(np.uint8)
		img3=img3.astype(np.uint8)
		sockClient=server.socketSend(sockClient,img_binary)

		if open==1:
			server.socketDisconnect(sockClient,sock)
			sys.exit()


	#/****************************************/

