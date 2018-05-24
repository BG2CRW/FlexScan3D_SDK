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
import tensorflow.contrib.slim as slim

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
		self.dropout = cfg.DROPOUT

		self.batch_size = cfg.BATCH_SIZE
		self.num_epochs = cfg.NUM_EPOCHS

		if mode==1:
			self.batch_size=1

		self.x = tf.placeholder(tf.float32, [self.batch_size, self.h, self.w, self.c])
		self.y = tf.placeholder(tf.float32, [self.batch_size, self.h, self.w, 1])

		self.build_model()
		self.loss()


	def train(self,old):
		#显示图像
		self.out1 = self.image_summary('feature', self.start, max_outputs=3)
		self.out2 = self.image_summary('result', self.out, max_outputs=3)
		self.summ_loss1	= self.scalar_summary('loss1', self.loss)

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
		self.summ_merge = self.merge_summary([self.out1, self.out2, self.summ_loss1])

		self.writer = self.SummaryWriter('./logs', self.sess.graph)
		
		image_path, label_path = self.read_image_path()
		list_images = list(range(len(image_path)))

		count = 0
		for epoch in xrange(self.num_epochs):
			random.shuffle(list_images)
			for batch in xrange(len(image_path)//self.batch_size):
				image_batch, label_batch = self.read_images(image_path, label_path, batch, list_images)
				err, _, summary_str = self.sess.run([self.loss, self.train_op, self.summ_merge], feed_dict={self.x:image_batch, self.y:label_batch})
				self.writer.add_summary(summary_str, count)

				count += 1

				print("Epoch: [%2d] [%4d/%4d], loss: %.8f" % (epoch, batch, len(image_path)//self.batch_size, err))

				#if count%20 == 0:
			self.saver.save(self.sess, save_path)


	def read_image_path(self):
		file_list = os.listdir(self.image_path)

		image_list = []
		label_list = []
		# for file in file_list:
		# 	image_fil_=os.path.join('%s\%s' % (self.image_path, file))
		# 	label_fil_=os.path.join('%s\%s' % (self.label_path, file))
		# 	image_fil_list=os.listdir(image_fil_)
		# 	for pic in image_fil_list:
		# 		image_list += [os.path.join('%s\%s' % (image_fil_, pic))]
		# 		label_list += [os.path.join('%s\%s' % (label_fil_, pic))]
		for pic in file_list:
			image_list += [os.path.join('%s/%s' % (self.image_path, pic))]
			label_list += [os.path.join('%s/%s' % (self.label_path, pic))]

		return image_list, label_list

	def read_images(self, image_path, label_path, batch, list_images):
		image = np.zeros([self.batch_size, self.h, self.w, self.c])
		label = np.zeros([self.batch_size, self.h, self.w, 1])
		for i in range(self.batch_size):
			if cfg.INPUT_CHANNEL==1:
				im = cv2.imread(image_path[list_images[batch*self.batch_size+i]],0)
			else:
				im = cv2.imread(image_path[list_images[batch*self.batch_size+i]])
			im = np.asarray(im)
			im = im.astype('float32')
			im = im.reshape(1,self.h,self.w,self.c)

			image[i] = im
			#print("image:%s"%image_path[list_images[batch*self.batch_size+i]])

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
		
		batch_norm_params = { 
        'is_training': True,
        'zero_debias_moving_mean': True,
        'decay': 0.999,
        'epsilon': 1e-3,
        'scale': True,
        'updates_collections': tf.GraphKeys.UPDATE_OPS,    
    	}

		#开始卷积
		net = slim.conv2d(self.x, 64, [3, 3], stride=1, padding='SAME', normalizer_fn=slim.batch_norm,normalizer_params=batch_norm_params,activation_fn=tf.nn.relu, scope='conv1_point')
		net = slim.avg_pool2d(net, [2, 2],stride=2,padding='SAME',scope='pool1_point')
		net = slim.conv2d(net, 128, [9, 9], stride=1, padding='SAME',normalizer_fn=slim.batch_norm,normalizer_params=batch_norm_params,activation_fn=tf.nn.relu, scope='conv21_point')
		net = slim.conv2d(net, 128, [1, 1], stride=1, padding='SAME',normalizer_fn=slim.batch_norm,normalizer_params=batch_norm_params,activation_fn=tf.nn.relu, scope='conv2_point')
		net = slim.avg_pool2d(net, [2, 2],stride=2, padding='SAME', scope='pool2_point') 
		net = slim.conv2d(net, 256, [9, 9], stride=1, padding='SAME',normalizer_fn=slim.batch_norm,normalizer_params=batch_norm_params,activation_fn=tf.nn.relu, scope='conv3_point')
		net = slim.conv2d(net, 256, [1, 1], stride=1, padding='SAME',normalizer_fn=slim.batch_norm,normalizer_params=batch_norm_params,activation_fn=tf.nn.relu, scope='conv31_point')	
		net = slim.conv2d(net, 64, [3, 3], stride=1, padding='SAME',normalizer_fn=slim.batch_norm,normalizer_params=batch_norm_params,activation_fn=tf.nn.relu, scope='conv6_point')	
		net = slim.conv2d(net, 1, [3, 3], stride=1, padding='SAME',normalizer_fn=None,normalizer_params=None,activation_fn=tf.nn.relu, scope='conv7_point')	
		print(net)

		#上采样
		net = self._deconv_layer(net, 1, int(self.h), int(self.w), kernel_h=9, kernel_w=9, strides_h=4, strides_w=4, name='deconv1_point')
		net = self._relu_layer(net, 'relu8_point')

		#net = slim.conv2d_transpose(net, 1, [9, 9], stride=4, padding='SAME',activation_fn=tf.nn.relu, scope='deconv1_point')	
		self.out = net

	def loss(self):
		self.loss = tf.losses.mean_squared_error(labels=self.y,predictions=self.out)

		#self.train_op = self._train_Adam(self.loss, self.learning_rate, beta1=0.5)
		self.train_op = tf.train.AdamOptimizer(learning_rate=self.learning_rate).minimize(self.loss)