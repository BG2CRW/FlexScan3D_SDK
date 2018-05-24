# encoding:utf-8
# path and dataset parameter
#
import os

TRAIN_OLD_STATUS=True   #true for continue train
GPU_NUMBER='0'

INPUT_HEIGHT=1200
INPUT_WIDTH=630
INPUT_CHANNEL=1

BATCH_SIZE=7
NUM_EPOCHS=200

IMAGE_PATH='data/outSrc'
LABEL_PATH='data/outLabel'
MODEL_PATH='model/scratch.ckpt'

LEARNING_RATE=(1e-3)*0.05
DROPOUT=0.7

