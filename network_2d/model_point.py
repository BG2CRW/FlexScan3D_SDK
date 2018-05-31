# encoding:utf-8
import tensorflow as tensorflow
import numpy as np
import cv2
import os
import time
from math import *
from six.moves import xrange
import random
import config as cfg

from network__ import *

class Rec_point(Network):

	def __init__(self, sess, run=0, mode=0):

		self.learning_rate = cfg.LEARNING_RATE

		self.image_summary = tf.summary.image
		self.scalar_summary = tf.summary.scalar
		self.histogram_summary = tf.summary.histogram
		self.merge_summary = tf.summary.merge
		self.SummaryWriter = tf.summary.FileWriter

		self.sess = sess
		self.Debug = True

		self.image_path = cfg.IMAGE_PATH
		self.label_path = cfg.LABEL_PATH
		self.h = cfg.INPUT_HEIGHT
		self.w = cfg.INPUT_WIDTH
		self.c = cfg.INPUT_CHANNEL
		self.ic = cfg.IMAGE_CHANNEL
		self.tc = cfg.TRAIN_CHANNEL
		self.th = cfg.TRAIN_HEIGHT
		self.tw = cfg.TRAIN_WIDTH

		self.batch_size = cfg.BATCH_SIZE
		self.num_epochs = cfg.NUM_EPOCHS

		if mode==1:
			self.batch_size=1

		self.x = tf.placeholder(tf.float32, [self.batch_size, self.th, self.tw, self.tc])
		self.y = tf.placeholder(tf.float32, [self.batch_size, self.th, self.tw, 1])


		self.img_in=tf.placeholder(tf.float32,[self.h, self.w, self.c])
		self.img_out=self.data_aug(self.img_in)

		self.build_model()
		self.loss()


	def train(self,old):
		#显示图像
		#self.out1 = self.image_summary('feature', self.start, max_outputs=3)
		#self.out2 = self.image_summary('result', self.out, max_outputs=3)
		#self.summ_loss1	= self.scalar_summary('loss1', self.loss)

		self.saver = tf.train.Saver()
		save_path = cfg.MODEL_PATH
		tf.global_variables_initializer().run()

		if old:
			self.saver.restore(self.sess, save_path)
			print('Finished reloading param.')
		else:
			print('Building a new model.')
			print('The former params will be dumped.')

		#申请图像内存
		#self.summ_merge = self.merge_summary([self.out1, self.out2, self.summ_loss1])

		#self.writer = self.SummaryWriter('./logs', self.sess.graph)

		image_path, label_path = self.read_image_path()
		image_path.sort()  #n*self.ic
		label_path.sort()  #n
		print(len(image_path),"  and  ",len(label_path))
		list_images = list(range(len(label_path)))  #n

		count = 0
		for epoch in xrange(self.num_epochs):
			random.shuffle(list_images)
			avg_loss = 0
			if (epoch+1) % 10 == 0:
				self.learning_rate = self.learning_rate * 0.6

			for batch in xrange(len(label_path)//self.batch_size):
				time1 = time.clock()
				image_batch, label_batch = self.read_images(image_path, label_path, batch, list_images)

				for arg_index in range(1):
					new_image_batch = np.zeros([self.batch_size, self.h, self.w, self.tc])

					for i in range(self.batch_size):
						for cc in range(self.tc):
							img = image_batch[i,:,:,cc]
							img = img.reshape(self.h, self.w, self.c)
							new_img = self.sess.run(self.img_out,feed_dict={self.img_in:img})
							new_img = new_img.reshape(self.h, self.w)
							new_image_batch[i,:,:,cc] = new_img
							#print(new_image_batch.shape)


					batch_part1 = new_image_batch[:,:,:self.tw,:]
					batch_part2 = new_image_batch[:,:,(self.w-self.tw):,:]
					label_part1 = label_batch[:,:,:self.tw,:]
					label_part2 = label_batch[:,:,(self.w-self.tw):,:]
					#print("batch part size is:",batch_part1.shape,"and",batch_part2.shape)

					err1, _ = self.sess.run([self.loss, self.train_op], feed_dict={self.x:batch_part1, self.y:label_part1})
					err2, _ = self.sess.run([self.loss, self.train_op], feed_dict={self.x:batch_part2, self.y:label_part2})
					#err1, _, summary_str = self.sess.run([self.loss, self.train_op, self.summ_merge], feed_dict={self.x:batch_part1, self.y:label_part1})
					#err2, _, summary_str = self.sess.run([self.loss, self.train_op, self.summ_merge], feed_dict={self.x:batch_part2, self.y:label_part2})



				#err, _, summary_str = self.sess.run([self.loss, self.train_op, self.summ_merge], feed_dict={self.x:image_batch, self.y:label_batch})
				#self.writer.add_summary(summary_str, count)

				count += 1

				err = err1 + err2
				time2 = time.clock()-time1
				print("Epoch: [%2d] [%4d/%4d], loss: %.4f, time:%.2f" % (epoch, batch, len(label_path)//self.batch_size, err, time2))
				avg_loss += err

				self.saver.save(self.sess, save_path)

			avg_loss = avg_loss/len(label_path)
			print("Avg_loss:",avg_loss)

		print("Train finished.")
		txt.close()


	def data_aug(self,image):
		image = tf.image.random_brightness(image, 32. / 255.)
		#image = tf.image.random_saturation(image, lower=0.5, upper=1.5)
		#image = tf.image.random_hue(image, max_delta=0.2)
		image = tf.image.random_contrast(image, lower=0.5, upper=1.5)
		image=tf.image.per_image_standardization(image)
		#print(image.shape)
		return image

	def PCA_Jittering(img):
		img_size = img.size/3
		#print(img.size, img_size)
		img1 = img.reshape(int(img_size), 3)
		img1 = np.transpose(img1)
		img_cov = np.cov([img1[0], img1[1], img1[2]])
		# 计算矩阵特征向量
		lamda, p = np.linalg.eig(img_cov)

		p = np.transpose(p)
		# 生成正态分布的随机数
		alpha1 = random.normalvariate(0, 0.2)
		alpha2 = random.normalvariate(0, 0.2)
		alpha3 = random.normalvariate(0, 0.2)

		v = np.transpose((alpha1 * lamda[0], alpha2 * lamda[1], alpha3 * lamda[2]))  # 加入扰动
		add_num = np.dot(p, v)
		img2 = np.array([img[:, :, 0] + add_num[0], img[:, :, 1] + add_num[1], img[:, :, 2] + add_num[2]])
		img2 = np.swapaxes(img2, 0, 2)
		img2 = np.swapaxes(img2, 0, 1)

		return img2


	def read_image_path(self):
		file_list1 = os.listdir(self.image_path)
		file_list2 = os.listdir(self.label_path)

		image_list = []
		label_list = []
		# for file in file_list:
		# 	image_fil_=os.path.join('%s\%s' % (self.image_path, file))
		# 	label_fil_=os.path.join('%s\%s' % (self.label_path, file))
		# 	image_fil_list=os.listdir(image_fil_)
		# 	for pic in image_fil_list:
		# 		image_list += [os.path.join('%s\%s' % (image_fil_, pic))]
		# 		label_list += [os.path.join('%s\%s' % (label_fil_, pic))]
		for pic in file_list1:
			image_list += [os.path.join('%s/%s' % (self.image_path, pic))]
		for pic in file_list2:
			label_list += [os.path.join('%s/%s' % (self.label_path, pic))]

		return image_list, label_list

	def read_images(self, image_path, label_path, batch, list_images):
		image = np.zeros([self.batch_size, self.h, self.w, self.ic])
		label = np.zeros([self.batch_size, self.h, self.w, 1])
		for i in range(self.batch_size):
			for cc in range(self.ic):
				if cfg.INPUT_CHANNEL==1:
					#print(image_path[list_images[batch*self.batch_size+i]*self.ic + cc])
					im = cv2.imread(image_path[list_images[batch*self.batch_size+i]*self.ic + cc],0)
				else: #unused
					im = cv2.imread(image_path[list_images[batch*self.batch_size+i]])
				im = np.asarray(im)
				im = im.astype('float32')
				im = im.reshape(1,self.h,self.w)
				#im = im.reshape(1,self.h,self.w,self.c)
				#print(im.shape)

				image[i,:,:,cc] = im
			#print("image:%s"%image_path[list_images[batch*self.batch_size+i]])

			#print(label_path[list_images[batch*self.batch_size+i]])
			la = cv2.imread(label_path[list_images[batch*self.batch_size+i]])
			la = np.asarray(la)
			la = la.astype('float32')
			la = la[0:,0:,0]
			la = la.reshape(1,self.h,self.w,1)

			label[i] = la

			#print("label:%s"%label_path[list_images[batch*self.batch_size+i]])
		return image, label

	def read_txts(self, image_path, batch, list_txts):
		txt = np.zeros([self.batch_size, 4, 2])
		lable = np.zeros([self.batch_size, 1])

		for i in range(self.batch_size):

			f = open(image_path[list_txts[batch*self.batch_size + i]])
			matrix = np.loadtxt(image_path[list_txts[batch*self.batch_size + i]])
			matrix = np.asarray(matrix)
			txt_each = matrix.reshape(self.h, self.w)
			f.close()
			txt[i] = txt_each

		return txt

	def build_model(self):
		self.start = self.x
		#开始卷积
		conv1_point = self._conv_layer(self.x, 64, 'conv1_point', kernel_h=3, kernel_w=3, strides_h=1, strides_w=1)
		norm1_point = self._batch_norm_layer(conv1_point, 'norm1_point')
		relu1_point = self._relu_layer(norm1_point, 'relu1_point')

		pool1_point = self._avg_pool_layer(relu1_point, 'pool1_point', kernel_h=2, kernel_w=2, strides_h=2, strides_w=2)

		conv21_point = self._conv_layer(pool1_point, 128, 'conv21_point', kernel_h=9, kernel_w=9, strides_h=1, strides_w=1)
		norm21_point = self._batch_norm_layer(conv21_point, 'norm21_point')
		relu21_point = self._relu_layer(norm21_point, 'relu21_point')

		conv2_point = self._conv_layer(relu21_point, 128, 'conv2_point', kernel_h=1, kernel_w=1, strides_h=1, strides_w=1)
		norm2_point = self._batch_norm_layer(conv2_point, 'norm2_point')
		relu2_point = self._relu_layer(norm2_point, 'relu2_point')

		pool2_point = self._avg_pool_layer(relu2_point, 'pool2_point', kernel_h=2, kernel_w=2, strides_h=2, strides_w=2)

		conv3_point = self._conv_layer(pool2_point, 256, 'conv3_point', kernel_h=9, kernel_w=9, strides_h=1, strides_w=1)
		norm3_point = self._batch_norm_layer(conv3_point, 'norm3_point')
		relu3_point = self._relu_layer(norm3_point, 'relu3_point')

		conv31_point = self._conv_layer(relu3_point, 256, 'conv31_point', kernel_h=1, kernel_w=1, strides_h=1, strides_w=1)
		norm31_point = self._batch_norm_layer(conv31_point, 'norm31_point')
		relu31_point = self._relu_layer(norm31_point, 'relu31_point')

		conv5_point = self._conv_layer(relu31_point, 512, 'conv5_point', kernel_h=9, kernel_w=9, strides_h=1, strides_w=1)
		norm5_point = self._batch_norm_layer(conv5_point, 'norm5_point')
		relu5_point = self._relu_layer(norm5_point, 'relu5_point')

		conv51_point = self._conv_layer(relu5_point, 512, 'conv51_point', kernel_h=1, kernel_w=1, strides_h=1, strides_w=1)
		norm51_point = self._batch_norm_layer(conv51_point, 'norm51_point')
		relu51_point = self._relu_layer(norm51_point, 'relu51_point')

		conv6_point = self._conv_layer(relu31_point, 64, 'conv6_point', kernel_h=3, kernel_w=3)
		norm6_point = self._batch_norm_layer(conv6_point, 'norm6_point')
		relu6_point = self._relu_layer(norm6_point, 'relu6_point')

		conv7_point = self._conv_layer(relu6_point, 1, 'conv7_point', kernel_h=3, kernel_w=3)
		relu7_point = self._relu_layer(conv7_point, 'relu7_point')

		#上采样
		deconv1_point = self._deconv_layer(relu7_point, 1, int(self.th), int(self.tw), kernel_h=9, kernel_w=9, strides_h=4, strides_w=4, name='deconv1_point')
		relu8_point = self._relu_layer(deconv1_point , 'relu8_point')

		self.out = relu8_point

	def loss(self):
		#self.y_2 = self._max_pool_layer(self.y, 'y_2', kernel_h=2, kernel_w=2, strides_h=2, strides_w=2)

		self.loss1 = self._mean_squared_error(self.y, self.out)
		#self.loss2 = self._mean_squared_error(self.y_2,self.relu7_point)

		self.loss = self.loss1 #+ self.loss2
		self.train_op = self._train_Adam(self.loss, self.learning_rate, beta1=0.5)