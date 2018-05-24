# encoding:utf-8
import tensorflow as tf
import numpy as np
from math import *
import os
import config as cfg
os.environ["CUDA_VISIBLE_DEVICES"] = cfg.GPU_NUMBER

from model_point import *

if __name__ == '__main__':

	with tf.Session() as sess:

		Rec = Rec_point(sess)
		Rec.train(old=cfg.TRAIN_OLD_STATUS)