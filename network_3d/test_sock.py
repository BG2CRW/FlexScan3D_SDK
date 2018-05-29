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

	sock=server.socketConnect(55)
	print("server build")
	while True:
		sockClient = server.accept(sock)
		sockClient,receiveImage,open=server.socketReceive(sockClient)
		#cv2.imshow("receiveImage",receiveImage);
		#cv2.waitKey(0)
		#test
		img1 = receiveImage.reshape(1, cfg.INPUT_HEIGHT, cfg.INPUT_WIDTH, cfg.INPUT_CHANNEL)#src
		img_gt1 = receiveImage.reshape(1, cfg.INPUT_HEIGHT, cfg.INPUT_WIDTH, 1)
		time1=time.clock()
		img2 = sess.run(model_point.out, feed_dict={model_point.x:img1})
		time2=time.clock()-time1
		print(time2,".....time")
		img3=img2[0, 0:, 0:, 0]
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
