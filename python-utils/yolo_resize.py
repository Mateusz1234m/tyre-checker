import cv2
import os
import shutil


def resize_obj(src_path, dest_size, dest_path=None):
    """
    Creates a new obj folder based on the original obj folder containing the original images in full (but different)
    resolution.
    :param src_path: path to the original obj folder
    :param dest_size: image the size to which the images will be resized, (width, height)
    :param dest_path: optional, path to the destination obj folder
    :return:
    """

    # get destination width and weight
    dest_w = dest_size[0]
    dest_h = dest_size[1]

    # define destination path if not specified as parameter
    if dest_path is None:
        _dest_path = f"{src_path[:-1]}_{str(dest_w)}_{str(dest_h)}/"
    else:
        _dest_path = dest_path

    # create a destination folder if it doesn't exist
    path_exist = os.path.exists(_dest_path)
    if not path_exist:
        os.mkdir(_dest_path)

    # get the number of files in the source obj folder
    no_files = len(os.listdir(src_path))

    # iterate over all files in the source obj folder
    for i, filename in enumerate(os.listdir(src_path)):

        # print current iteration
        print(f"File: {i + 1}/{no_files}")

        # copy the file to the destination obj folder if it is a label
        if filename[-4:] == ".txt":
            shutil.copyfile(src_path + filename, _dest_path + filename)

        # read, resize and write the file to the destination obj folder if it is an image
        elif filename[-4:] == ".png":
            img = cv2.imread(src_path + filename)
            img = cv2.resize(img, (dest_w, dest_h))
            cv2.imwrite(_dest_path + filename, img)


if __name__ == "__main__":

    # resize textfields obj
    resize_obj("C:/Users/mateu/Desktop/projekty/dataset_opony/obj_textfields/", (1216, 192))

    # resize characters obj
    resize_obj("C:/Users/mateu/Desktop/projekty/dataset_opony/obj_characters/", (96, 64))
