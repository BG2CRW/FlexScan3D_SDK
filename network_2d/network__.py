# encoding:utf-8
import tensorflow as tf
import numpy as np
from math import *
import config as cfg

class Network():

	#def __init__(self):


	def _dense_layer(self, bottom, out_channels, name, biases_initializer=tf.zeros_initializer(), trainable=True, reuse=False):
		with tf.variable_scope(name):

			weights = tf.get_variable('dense_weights', shape=[int(bottom.shape[-1]), out_channels],
													   dtype=bottom.dtype,
													   initializer=tf.constant_initializer(0.1),
													   trainable=trainable)
			biases = tf.get_variable('dense_biases', shape=[out_channels],
													 dtype=bottom.dtype,
													 initializer=tf.constant_initializer(0.1),
													 trainable=trainable)
			dense = tf.matmul(bottom, weights) + biases
			# Adding to the tensorboard
			tf.summary.histogram(name+'/dense_weights', weights)
			tf.summary.histogram(name+'/dense_biases', biases)

		if self.Debug:
			print(dense)

		return dense


	def _conv_layer(self, bottom, out_channels, name,kernel_h=3, kernel_w=3, strides_h=1, strides_w=1,stddev=0.1, padding='SAME', trainable=True, reuse=False):
		with tf.variable_scope(name):

			weights = tf.get_variable('conv_weights', shape=[kernel_h,kernel_w,int(bottom.shape[-1]),out_channels],
													  dtype=bottom.dtype,
													  initializer=tf.random_normal_initializer(stddev=stddev),
													  trainable=trainable)
			biases = tf.get_variable('conv_biases', shape=[out_channels],
												  dtype=bottom.dtype,
												  initializer=tf.random_normal_initializer(stddev=stddev),
												  trainable=trainable)
			conv = tf.nn.convolution(bottom, weights, strides = [strides_h,strides_w], padding=padding) + biases
			# Adding to the tensorboard
			tf.summary.histogram(name+"/conv_weights", weights)
			tf.summary.histogram(name+"/conv_biases", biases)

		if self.Debug:
			print(conv)

		return conv


	def _tanh_layer(self, bottom, name, reuse=False):
		with tf.variable_scope(name):
			return tf.nn.tanh(bottom, name='tanh')


	def _sigmoid_layer(self, bottom, name, reuse=False):
		with tf.variable_scope(name):
			return tf.nn.sigmoid(bottom, name='sigmoid')


	def _relu_layer(self, bottom, name, reuse=False):
		with tf.variable_scope(name):
			return tf.nn.relu(bottom, name='relu')


	def _leaky_relu_layer(self, bottom, name, leak=0.2, reuse=False):
		with tf.variable_scope(name):
			return tf.maximum(bottom, leak*bottom)


	def _parametric_relu_layer(self, bottom, name, trainable=True, reuse=False):
		with tf.variable_scope(name):
			_alpha = tf.get_variable('prelu', shape=int(bottom.shape[-1]),
											  dtype=bottom.dtype,
											  initializer=tf.constant_initializer(0.1),
											  trainable=trainable)
			return tf.maximum(0, bottom) + _alpha*tf.minimum(0, bottom)


	def _swish_layer(self, bottom, name, reuse=False):
		with tf.variable_scope(name):
			return bottom*tf.nn.sigmoid(bottom, name='swish')


	def _max_pool_layer(self, bottom, name, kernel_h=2, kernel_w=2, strides_h=2, strides_w=2, reuse=False):
		with tf.variable_scope(name):
			pool = tf.nn.max_pool(bottom, ksize=[1,kernel_h,kernel_w,1],
										  strides=[1,strides_h,strides_w,1],
										  padding = 'SAME', name = 'max_pooling')

		if self.Debug:
			print(pool)

		return pool


	def _avg_pool_layer(self, bottom, name, kernel_h=2, kernel_w=2, strides_h=2, strides_w=2, reuse=False):
		with tf.variable_scope(name):
			pool = tf.nn.avg_pool(bottom, ksize=[1,kernel_h,kernel_w,1],
										  strides=[1,strides_h,strides_w,1],
										  padding = 'SAME', name = 'max_pooling')

		if self.Debug:
			print(pool)

		return pool


	def _reshape_layer(self, bottom, shape, name, reuse=False):
		with tf.variable_scope(name):
			reshape = tf.reshape(bottom, shape)

		if self.Debug:
			print(reshape)

		return reshape


	def _add_layer(self, bottom_1, bottom_2, name, reuse=False):
		with tf.variable_scope(name):
			add = tf.add(bottom_1, bottom_2)

		if self.Debug:
			print(add)

		return add


	def _deconv_layer_fcn(self, bottom, out_height, out_width, strides_xy, name, out_channels=-1, reuse=False):
		if out_channels == -1:
			out_channels = self.num_classes

		with tf.variable_scope(name):
			weights = self.__get_deconv_filter(name, f_shape=[strides_xy*2,strides_xy*2,out_channels,out_channels])
			upscore = tf.nn.conv2d_transpose(bottom, weights,
											 [tf.shape(bottom)[0],out_height,out_width,out_channels],
											 [1,strides_xy,strides_xy,1], padding='SAME', data_format='NHWC', name=None)

		if self.Debug:
			print(upscore)

		return upscore


	def _deconv_layer(self, bottom, out_channels, out_height, out_width, kernel_h, kernel_w, strides_h, strides_w, name, stddev=0.2, reuse=False):
		with tf.variable_scope(name):
			weights = tf.get_variable(name+'/deconv_weights', [kernel_h, kernel_w, out_channels, int(bottom.shape[-1])],
									  dtype=bottom.dtype,
									  initializer=tf.random_normal_initializer(stddev=stddev))
			biases = tf.get_variable(name+'/deconv_biases', [out_channels], initializer=tf.constant_initializer(0.0))

			deconv = tf.nn.conv2d_transpose(bottom, weights, [int(bottom.shape[0]),out_height, out_width, out_channels],
															 [1, strides_h, strides_w, 1]) + biases

			tf.summary.histogram(name+"/deconv_weights", weights)
			tf.summary.histogram(name+"/deconv_biases", biases)

		if self.Debug:
			print(deconv)

		return deconv


	def _dropout_layer(self, bottom, name, reuse=False):
		with tf.variable_scope(name):
			dropout = tf.nn.dropout(bottom, 0.5)

		return dropout


	def _batch_norm_layer(self, bottom, name, momentum=0.999,
											  epsilon=1e-3,
											  scale=True,
											  is_training=True,
											  trainable=True,
											  reuse=False):
		with tf.variable_scope(name):
			batch_norm = tf.contrib.layers.batch_norm(bottom,
													  decay=momentum,
													  epsilon=epsilon,
													  scale=scale,
													  is_training=trainable,
													  trainable=trainable,
													  scope=name+'batch_norm')

		if self.Debug:
			print(batch_norm)

		return batch_norm

	'''
	def _batch_norm_layer_(self, bottom, name, momentum=0.999,
											  epsilon=1e-3,
											  scale=True,
											  is_training=False,
											  trainable=False,
											  reuse=False):
		with tf.variable_scope(name) as scope:
			if reuse:
				scope.reuse_variables()
			batch_norm = tf.contrib.layers.batch_norm(bottom,
													  decay=momentum,
													  epsilon=epsilon,
													  scale=scale,
													  is_training=trainable,
													  trainable=trainable,
													  scope=name+'batch_norm')

		if self.Debug:
			print(batch_norm)

		return batch_norm
	'''


	def __get_deconv_filter(self, f_name, f_shape):
		width = f_shape[0]
		heigh = f_shape[0]
		f = ceil(width/2.0)
		c = (2 * f - 1 - f % 2) / (2.0 * f)
		bilinear = np.zeros([f_shape[0], f_shape[1]])
		for x in range(width):
			for y in range(heigh):
				value = (1 - abs(x / f - c)) * (1 - abs(y / f - c))
				bilinear[x, y] = value
		weights = np.zeros(f_shape)
		for i in range(f_shape[2]):
			weights[:, :, i, i] = bilinear

		init = tf.constant_initializer(value=weights, dtype=tf.float32)
		var = tf.get_variable(name = f_name, initializer=init, shape=weights.shape)

		return var


	def _softmax_cross_entropy_with_logits(self, y, prediction):
		logits = tf.nn.softmax_cross_entropy_with_logits(labels=y,
														 logits=prediction,
														 name="softmax_cross_entropy")

		return logits


	def _sparse_softmax_cross_entropy_with_logits(self, y, prediction):
		logits = tf.nn.sparse_softmax_cross_entropy_with_logits(labels=y,
																logits=prediction,
																name="sparse_softmax_cross_entropy")

		return logits


	def _sigmoid_cross_entropy_with_logits(self, y, prediction):
		logits = tf.nn.sigmoid_cross_entropy_with_logits(labels=y,
														 logits=prediction,
														 name="sigmoid_cross_entropy")

		return logits


	def _mean_squared_error(self, y, prediction):
		logits = tf.losses.mean_squared_error(labels=y,
											  predictions=prediction)

		return logits

	def _pos_error(self, prediction, batch_size):  #位置回归的损失函数
		loss = 0.0
		for j in range(batch_size):
			loss = loss + tf.abs(prediction[j,0] - 1)
		return loss


	def _loss(self, logits):
		cross_entropy = tf.reduce_mean(logits)
		tf.summary.scalar("loss",cross_entropy)

		return cross_entropy


	def _train_RMSProp(self, loss, learning_rate, decay=0.9,
												  momentum=0.0,
												  epsilon=1e-10,
												  centered=False,
												  var_list=None):
		optimizer = tf.train.RMSPropOptimizer(learning_rate, decay=decay,
															 momentum=momentum,
															 epsilon=epsilon,
															 centered=centered)
		train_op = optimizer.minimize(loss, var_list=var_list)

		return train_op


	def _train_Adam(self, loss, learning_rate, beta1=0.9,
											   beta2=0.999,
											   epsilon=1e-8,
											   var_list=None):
		optimizer = tf.train.AdamOptimizer(learning_rate, beta1=beta1,
														  beta2=beta2,
														  epsilon=epsilon)
		train_op = optimizer.minimize(loss, var_list=var_list)

		return train_op


	def _train_GradientDescent(self, loss, learning_rate, var_list=None):
		optimizer = tf.train.GradientDescentOptimizer(learning_rate)
		train_op = optimizer.minimize(loss, var_list=var_list)

		return train_op