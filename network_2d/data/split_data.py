# -*- coding: utf-8 -*-

import os
import random
import PIL.Image as Image
import shutil



# 返回文件列表
def eachFile(filepath):
    pathDir = os.listdir(filepath)
    child_file_name = []
    full_child_file_list = []
    for allDir in pathDir:
        if not allDir == '.DS_Store':
            child = os.path.join(filepath, allDir)

            full_child_file_list.append(child)
            child_file_name.append(allDir)

    child_file_name.sort()
    full_child_file_list.sort()


    return full_child_file_list, child_file_name


# 转移ratio文件
def move_ratio(data_list, original_str, replace_str):
    for x in data_list:
        shutil.move(x,replace_str)


if __name__ == '__main__':
    label_path = 'trainLabel/'
    src_path = 'trainSrc/'
    test_src_path = 'testSrc/'
    test_label_path = 'testLabel/'

    index = []
    for i in range(646):
        index.append(i)
    random.shuffle(index)
    print(index)

    #read label
    full_child_file, child_file = eachFile(label_path)
    full_src_file, src_file = eachFile(src_path)

    print(len(full_child_file))
    print(len(full_src_file))

    sfull_child_file = []
    sfull_src_file = []
    for i in range(646):
        sfull_child_file.append(full_child_file[index[i]])
        sfull_src_file.append(full_src_file[index[i]*5])
        sfull_src_file.append(full_src_file[index[i]*5+1])
        sfull_src_file.append(full_src_file[index[i]*5+2])
        sfull_src_file.append(full_src_file[index[i]*5+3])
        sfull_src_file.append(full_src_file[index[i]*5+4])


    print(sfull_child_file)
    print(sfull_src_file)

    test_label_list = sfull_child_file[0 : 130]
    test_src_list = sfull_src_file[0 : 130 * 5]

    # train_move, val_move
    move_ratio(test_label_list, label_path, test_label_path)
    move_ratio(test_src_list, src_path, test_src_path)

