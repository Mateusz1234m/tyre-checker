import os
import random

def train_test_split(obj_path, dest_path, yolo_path, ratio, train_txt_name="train.txt", test_txt_name="test.txt"):
    """
    Creates .txt files with a list of training and test images.
    :param obj_path: path to the obj folder
    :param dest_path: path to the folder, where .txt files will be saved
    :param yolo_path: path to the obj folder in darknet framework
    :param ratio: train/test split ratio
    :param train_txt_name: name of the file with the list of training images
    :param test_txt_name: name of the file with the list of test images
    :return:
    """

    # get list of all files in the obj folder
    files_list = os.listdir(obj_path)

    # get list of images files in the obj folder
    img_list = []
    for file in files_list:
        if file[-4:] in [".png", ".jpg"]:
            img_list.append(file)

    # shuffle the list of labels to arrange them in random order
    random.shuffle(img_list)

    # split list of images into train and valid
    train_list = img_list[:int(((len(img_list) + 1) * ratio))]
    test_list = img_list[int(((len(img_list) + 1) * ratio)):]

    # create the file with training list
    with open(dest_path + train_txt_name, 'w') as f:
        for x in train_list:
            f.write(yolo_path + str(x) + '\n')

    # create the file with test list
    with open(dest_path + test_txt_name, 'w') as f:
        for x in test_list:
            f.write(yolo_path + str(x) + '\n')


if __name__ == "__main__":

    # split textfields dataset
    train_test_split(obj_path="C:/Users/mateu/Desktop/projekty/dataset_opony/obj_textfields/",
                     dest_path="C:/Users/mateu/Desktop/projekty/dataset_opony/pliki_konfiguracyjne/textfields"
                               "/train_textfields.txt",
                     yolo_path="/darknet/data/obj_textfields_1216_192/",
                     ratio=0.9, train_txt_name="train_textfields.txt", test_txt_name="valid_textfields.txt")

    # split characters dataset
    train_test_split(obj_path="C:/Users/mateu/Desktop/projekty/dataset_opony/obj_characters/",
                     dest_path="C:/Users/mateu/Desktop/projekty/dataset_opony/pliki_konfiguracyjne/characters"
                               "/train_characters.txt",
                     yolo_path="/darknet/data/obj_characters_96_64/",
                     ratio=0.9, train_txt_name="train_characters.txt", test_txt_name="valid_characters.txt")
        