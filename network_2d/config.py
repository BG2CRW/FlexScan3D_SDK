# encoding:utf-8
# path and dataset parameter
#
import os

TRAIN_OLD_STATUS=True   #true for continue train
OLD_EPOCHS = 200
GPU_NUMBER='1'

INPUT_HEIGHT=2168  #1200  1422  2168
INPUT_WIDTH=4096  #1600  2688  4096
INPUT_CHANNEL=1

IMAGE_CHANNEL=5
TRAIN_CHANNEL=5
TRAIN_HEIGHT=2168
TRAIN_WIDTH=2248


BATCH_SIZE=1
NUM_EPOCHS=30

IMAGE_PATH='data/data524/trainSrc'
LABEL_PATH='data/data524/trainLabel'
MODEL_PATH='model/data524/battery.ckpt'
SAVE_PATH='model/data524'
#TEST_PATH='data/test'
TEST_PATH='data/data524/testSrc'
TESTLABEL_PATH='data/data524/testLabel'

LEARNING_RATE=(1e-3)*0.5

