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
	data_num = len(train_image)
	avg_loss=0

	sum_edgepoint = 0
	sum_wujian = 0
	sum_loujian = 0
	sample_wujian = 0
	sample_loujian = 0

	for k in range(int(data_num)):
		if cfg.INPUT_CHANNEL==1:
			img=cv2.imread(train_image[k],0)
		else:
			img=cv2.imread(train_image[k])
		lab=cv2.imread(train_label[k])
		lab2 = cv2.cvtColor(lab, cv2.COLOR_BGR2GRAY)
		cv2.imshow("label",lab2)
		cv2.imshow("iput",img)
		bi_thre_lab2, lab2 = cv2.threshold(lab2, 40, 255, cv2.THRESH_BINARY)
		kernel02 = np.ones((2,2),np.uint8)
		lab2 = cv2.dilate(lab2,kernel02,1)
		lab2 = lab2.astype('uint8')
		_, contours_lab, hierarchy_lab = cv2.findContours(lab2.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
		num_edgepoint = len(contours_lab)
		sum_edgepoint += num_edgepoint

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
		#np.savetxt('n.txt',img3)
		#cv2.imshow("result",img3)

		kernel_point = np.ones((3,3),np.uint8)
		img_erosion_right = cv2.erode(img3, kernel_point, iterations = 1)
		img_dilation_right = cv2.dilate(img_erosion_right, kernel_point, iterations = 1)
		#cv2.namedWindow('img_e_d', 1)
		cv2.imshow('img_e_d', img_dilation_right)
		bi_thre_right, img_binary_right = cv2.threshold(img_dilation_right, 60, 255, cv2.THRESH_BINARY)
		cv2.imshow("66666666",img_binary_right)
		#kernel_2 = np.ones((3,3),np.uint8)
		#img_binary_right = cv2.dilate(img_binary_right, kernel02,1)
		#img_binary_right = cv2.erode(img_binary_right, kernel02,1)
		#img_binary_right = cv2.erode(img_binary_right, kernel_2,1)
		#img_binary_right = cv2.dilate(img_binary_right, kernel_2,1)
		#cv2.namedWindow('img_binary_right', 1)
		#cv2.imshow('img_binary_right', img_binary_right)
		img_binary_right = img_binary_right.astype('uint8')
		_, contours_right, hierarchy_right = cv2.findContours(img_binary_right.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
		#cv2.drawContours(img,contours_right,-1,(0,255,255),1)
		img666 = img
		#img666 = cv2.cvtColor(img666, cv2.COLOR_GRAY2BGR)
		contours_final =[]
		for c in range(len(contours_right)):
			cnt_right = contours_right[c]
			if cv2.contourArea(cnt_right) < 150:
				#print(cnt_right)
				continue
			contours_final.append(cnt_right)
			x_right, y_right ,w_right, h_right = cv2.boundingRect(cnt_right)
			cv2.rectangle(img666, (x_right,y_right), (x_right+w_right, y_right+h_right), (255,0,0), 2)
		cv2.namedWindow('right', 1)
		cv2.imshow('right', img666)
		resultPath = "F:/Tensorflow_ws/fcn_batt_2d/result/" + '%d' %k + "jpg"
		cv2.imwrite("result/%s"%dir_[k], img666)

		#将筛选出的凹坑画在黑色底图上
		num_detectpoint = len(contours_final)
		img_final = np.zeros((cfg.INPUT_HEIGHT,cfg.INPUT_WIDTH),np.uint8)
		cv2.drawContours(img_final,contours_final,-1,(255,0,0),-1)
		#cv2.imshow("final",img_final)
		#将label图膨胀，取反，再与缺陷做与操作，得到误检图
		kernel = np.ones((225,225),np.uint8)
		erosion_lab = cv2.dilate(lab2,kernel,1)
		not_lab = cv2.bitwise_not(erosion_lab)
		result_lab = cv2.bitwise_and(not_lab, img_final)
		#cv2.imshow("result_lab", result_lab)
		_, contours_result, hierarchy_result = cv2.findContours(result_lab.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
		num_diff = 0
		for cc in range(len(contours_result)):
			cnt_result = contours_result[cc]
			if cv2.contourArea(cnt_result) < 150:
				continue
			num_diff += 1
		#num_diff = len(contours_result)
		num_wujian = num_diff
		#计算漏检并累加
		print(num_edgepoint,"  and  ",num_detectpoint)
		num_loujian = num_edgepoint - (num_detectpoint - num_diff)
		sum_wujian += num_wujian
		sum_loujian += num_loujian
		print(num_loujian,"and",num_wujian)
		if num_edgepoint == 0:
			if num_detectpoint > 0:
				sample_wujian += 1
				continue
		if num_edgepoint >= 1 and num_edgepoint <= 2:
			if num_loujian > 0:
				sample_loujian += 1
				continue
			if num_wujian > 1:
				sample_wujian += 1
				continue
		if num_edgepoint < 5:
			if num_loujian > 1:
				sample_loujian += 1
				continue
			if num_wujian > 2:
				sample_wujian += 1

			# if (num_wujian/num_edgepoint) > 0.4:
			# 	if num_edgepoint < 5:
			# 		sample_wujian += 1
			# 		continue
			# if (num_loujian/num_edgepoint) > 0.3:
			# 	if num_detectpoint < 5:
			# 		sample_loujian += 1

		#cv2.waitKey()

	avg_loss /= data_num
	print(avg_loss,".....avg_loss")
	print("Edge Point number: ",sum_edgepoint)
	print("凹坑漏检数:",sum_loujian,",凹坑漏检率:",round(sum_loujian/sum_edgepoint,5)*100,"%")
	print("凹坑误检数:",sum_wujian,",凹坑误检率:",round(sum_wujian/sum_edgepoint,5)*100,"%")
	print("样本漏检数:",sample_loujian,",样本漏检率:",round(sample_loujian/data_num,5)*100,"%")
	print("样本误检数:",sample_wujian,",样本误检率:",round(sample_wujian/data_num,5)*100,"%")
	cv2.waitKey()