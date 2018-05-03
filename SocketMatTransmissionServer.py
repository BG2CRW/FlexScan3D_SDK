# coding:utf-8
import numpy as np
import tensorflow as tf
import cv2
import socket
import time
import random
from PIL import Image
import scipy
import struct
BUFFER_SIZE_LIMIT = 1930000

def socketConnect(port):
	sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM) 
	sock.bind(("127.0.0.1",port)) 
	sock.listen(5) 
	return sock

def socketReceive(sock):
	
	sockClient,addr = sock.accept() 
	print ('address:', addr)
	s=sockClient.recv(100)
	signal,pkg_num,channel,rows,cols=struct.unpack("<5I",s)
	small_pkg = int(rows*cols / BUFFER_SIZE_LIMIT + 1)
	#print(signal,pkg_num,channel,rows,cols)
	
	if small_pkg==1:
		img=np.zeros((rows,cols),dtype=np.uint8)
	for i in range(pkg_num):
		for j in range(small_pkg):
			packet=sockClient.recv(BUFFER_SIZE_LIMIT)
	
	for h in range(rows):
		for w in range (cols):
			img[h][w]=packet[h*cols+w]
	return sockClient,img,signal

def socketSend(sockClient,imgSend):
	s_rows,s_cols=imgSend.shape
	s_signal=0
	s_pkg_num=0
	s_channel=1
	print(s_rows,s_cols)
	ss=struct.pack("<5I",s_signal,s_pkg_num,s_channel,s_rows,s_cols)
	print(ss)
	sockClient.send(ss)
	packet=np.zeros(s_cols*s_rows,dtype=np.uint8)
	for h in range(s_rows):
		for w in range (s_cols):
			packet[h*s_cols+w]=imgSend[h][w]
	sockClient.send(packet)
	return sockClient

def socketDisconnect(sockClient,sock):
	sockClient.close() 
	sock.close()
