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

import threading

global listend
listend=[False, False, False, False, False, False]
global openlist
openlist =[]

def creat(num):
	listend[num]=False 
	sock=server.socketConnect(num)
	print(str(num)+"*")

	sockClient = server.accept(sock)
	sockClient.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
	print(str(num)+"**")
	
	sockClient,receiveImage,open = server.socketReceive(sockClient, num)
	#openlist.append(open)
	print(str(num)+"***")
	receiveImage = np.asarray(receiveImage)
	receiveImage = receiveImage.astype('float32')
	receiveImage = receiveImage.reshape(1,cfg.INPUT_HEIGHT,cfg.INPUT_WIDTH)
	img1[0,:,:,num-1] = receiveImage
	listend[num]=True
	print(str(num)*20)
	time.sleep(2)
	if open==1:
		server.socketDisconnect(sockClient,sock)
		sys.exit()


with tf.Session() as sess:

	global img1
	img1 = np.zeros([1,cfg.INPUT_HEIGHT,cfg.INPUT_WIDTH,cfg.IMAGE_CHANNEL])

	#/****************************************/
	threads = []
	t1 = threading.Thread(target=creat,args=(1,))
	threads.append(t1)
	t2 = threading.Thread(target=creat,args=(2,))
	threads.append(t2)
	t3 = threading.Thread(target=creat,args=(3,))
	threads.append(t3)
	t4 = threading.Thread(target=creat,args=(4,))
	threads.append(t4)
	t5 = threading.Thread(target=creat,args=(5,))
	threads.append(t5)
	for t in threads:
		t.setDaemon(True)
		t.start()
	while True:
		#img1 = np.zeros([1,cfg.INPUT_HEIGHT,cfg.INPUT_WIDTH,cfg.IMAGE_CHANNEL])
		for a in range(5):
			if not threads[a].isAlive():
				print(a)
				print("&&&&&&&&&&&&&&&&&&&&&&")
				threads[a] = threading.Thread(target=creat,args=(a+1,))
				threads[a].start()
		for t in threads:
			t.join()
		print("qqqqqqqqqqqqqqqqqqqq")
		while(True):
			if listend[1]==True and listend[2]==True and listend[3]==True and listend[4]==True and listend[5]==True:
				break
		print(img1.shape)



	#/****************************************/

