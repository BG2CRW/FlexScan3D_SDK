# encoding:utf-8
# path and dataset parameter
#
import os

TRAIN_OLD_STATUS=False   #true for continue train
GPU_NUMBER='0'

INPUT_HEIGHT=2168  #1200  1422  2168
INPUT_WIDTH=4096  #1600  2688  4096
INPUT_CHANNEL=1

IMAGE_CHANNEL=5
TRAIN_CHANNEL=5
TRAIN_HEIGHT=2168
TRAIN_WIDTH=2248


BATCH_SIZE=1
NUM_EPOCHS=10

IMAGE_PATH='data/data524/trainSrc'
LABEL_PATH='data/data524/trainLabel'
MODEL_PATH='model/data524/scratch.ckpt'
TEST_PATH='data/scratch519/testSrc'
TESTLABEL_PATH='data/scratch519/testLabel'

LEARNING_RATE=(1e-3)*0.05

