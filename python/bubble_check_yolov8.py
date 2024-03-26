# coding:utf-8
 
import os
import sys
import random
import argparse
import xml.etree.ElementTree as ET
from pathlib import Path
from ultralytics import YOLO
from PIL import Image

# 原始图像路径
orig_image_path = os.getcwd() + '/images_orig'
# 数据集路径
dataset_path = os.getcwd() + '/datasets'

# 目标预测图像路径
detect_pred_image_path = os.getcwd() + '/images_pred/detect'
# 原始标注路径
detect_xml_path = os.getcwd() + '/detect_xmls'
# 图像路径
detect_image_path = dataset_path + '/detect/images'
# 标注路径
detect_label_path = dataset_path + '/detect/labels'

# 分类原始图像路径
classify_orig_image_path = os.getcwd() + '/images_classify'
# 分类预测图像路径
classify_pred_image_path = os.getcwd() + '/images_pred/classify'
# 图像路径
classify_image_path = dataset_path + '/classify/images'


# 创建文件夹
if not Path(dataset_path).exists():
    Path(dataset_path).mkdir(parents=True)

if not Path(detect_pred_image_path).exists():
    Path(detect_pred_image_path).mkdir(parents=True)
if not Path(detect_image_path).exists():
    Path(detect_image_path).mkdir(parents=True)
if not Path(detect_label_path).exists():
    Path(detect_label_path).mkdir(parents=True)

if not Path(classify_pred_image_path).exists():
    Path(classify_pred_image_path).mkdir(parents=True)
if not Path(classify_image_path).exists():
    Path(classify_image_path).mkdir(parents=True)

# 数据集划分
datasets = [
    {
        'name': 'train',
        'percent': 0.5,
    },
    {
        'name': 'val',
        'percent': 0.3,
    },
    {
        'name': 'test',
        'percent': 0.2,
    },
]

classify_datas = [
    {
        'name': 'transparent',
        'filters': ['Bubble'] + [f'Trans{i}.bmp' for i in range(6)],
    },
    {
        'name': 'bubble',
        'filters': [f'Trans{i}.bmp' for i in range(7, 9)],
    }
]


def copy_file(from_path, to_path):
    if not Path(from_path).exists():
        return
    if Path(to_path).exists():
        return
    
    file_from = open(from_path, 'rb')
    file_to = open(to_path, 'wb')
    
    file_to.write(file_from.read())

    file_from.close()
    file_to.close()


def remove_file_or_folder(path):
    if not Path(path).exists():
        return
    
    if Path(path).is_dir():
        for file in Path(path).iterdir():
            remove_file_or_folder(file)
        Path(path).rmdir()
    else:
        Path(path).unlink()


def convert_label(width, height, xmin, ymin, xmax, ymax):
    # 标注越界修正
    xmin = min(xmin, width)
    xmax = min(xmax, width)
    ymin = min(ymin, height)
    ymax = min(ymax, height)

    xmin = max(xmin, 0)
    xmax = max(xmax, 0)
    ymin = max(ymin, 0)
    ymax = max(ymax, 0)

    x = (xmin + xmax) / 2 - 1
    y = (ymin + ymax) / 2 - 1
    w = xmax - xmin
    h = ymax - ymin
    
    x = x / width
    w = w / width
    y = y / height
    h = h / height
    return x, y, w, h
 

def convert_xml2txt(dataset, image_name):
    # 将xml标记文件转为txt标记文件
    in_file = open(f'{detect_xml_path}/{image_name}.xml', 'r', encoding='UTF-8')
    out_file = open(f'{detect_label_path}/{dataset}/{image_name}.txt', 'w', encoding='UTF-8')

    tree = ET.parse(in_file)
    root = tree.getroot()
    size = root.find('size')
    width = int(size.find('width').text)
    height = int(size.find('height').text)
    
    classes = ["c"]   # 改成自己的类别
    
    for obj in root.iter('object'):
        difficult = obj.find('difficult').text
        class_name = obj.find('name').text
        if class_name not in classes or int(difficult) == 1:
            continue
        cls_id = classes.index(class_name)
        
        xmlbox = obj.find('bndbox')
        xmin = float(xmlbox.find('xmin').text)
        xmax = float(xmlbox.find('xmax').text)
        ymin = float(xmlbox.find('ymin').text)
        ymax = float(xmlbox.find('ymax').text)
        
        x, y, w, h = convert_label(width, height, xmin, ymin, xmax, ymax)
        
        out_file.write(f'{cls_id} {x} {y} {w} {h}\n')
    
    in_file.close()
    out_file.close()


def detect_copy_image(data_set, image_name):
    # 将标记图像文件复制到图像集文件夹
    image_file_from = f'{orig_image_path}/{image_name}.bmp'
    image_file_to = f'{detect_image_path}/{data_set}/{image_name}.bmp'
    copy_file(image_file_from, image_file_to)


def detect_prepare_data():
    # 清空文件
    for dataset_name in Path(detect_image_path).iterdir():
        remove_file_or_folder(dataset_name)
    for dataset_name in Path(detect_label_path).iterdir():
        remove_file_or_folder(dataset_name)
    
    # xml标记文件列表
    xmls = os.listdir(detect_xml_path)
    random.shuffle(xmls)
    len_xmls = len(xmls)
    
    for dataset in datasets:
        dataset_name = dataset['name']

        if not Path(detect_image_path).joinpath(dataset_name).exists():
            Path(detect_image_path).joinpath(dataset_name).mkdir(parents=True)

        if not Path(detect_label_path).joinpath(dataset_name).exists():
            Path(detect_label_path).joinpath(dataset_name).mkdir(parents=True)

        # 截取数据集
        dataset_num = int(len_xmls * dataset['percent'])
        dataset_xmls = xmls[:dataset_num]
        xmls = xmls[dataset_num:]
        
        for xml in dataset_xmls:
            # 文件名
            image_name = xml.replace('.xml', '')
            print(dataset_name, image_name)
            # xml标记文件转为txt标记文件
            convert_xml2txt(dataset_name, image_name)
            # 将标记过的图像文件复制到图像集文件夹中
            detect_copy_image(dataset_name, image_name)


def classify_rough_split_images():
    for f in Path(classify_orig_image_path).iterdir():
        print(f)
        for classify_data in classify_datas:
            match = False
            for classify_filter in classify_data['filters']:
                if classify_filter in f.name:
                    classify_file = f.parent.joinpath(classify_data['name']).joinpath(f.name)
                    f.rename(classify_file)
                    match = True
                    break
            if match:
                break


def classify_copy_image(data_set, class_name, image_name):
    # 将标记图像文件复制到图像集文件夹
    image_file_from = f'{classify_orig_image_path}/{class_name}/{image_name}.bmp'
    image_file_to = f'{classify_image_path}/{data_set}/{class_name}/{image_name}.bmp'
    copy_file(image_file_from, image_file_to)


def classify_prepare_data():
    # 清空文件
    for dataset_name in Path(classify_image_path).iterdir():
        remove_file_or_folder(dataset_name)
    
    for classify_data in classify_datas:        
        # xml标记文件列表
        class_name = classify_data['name']
        class_images = os.listdir(classify_orig_image_path + '/' + class_name)
        random.shuffle(class_images)
        len_images = len(class_images)
        
        for dataset in datasets:
            dataset_name = dataset['name']

            if not Path(classify_image_path).joinpath(dataset_name).joinpath(class_name).exists():
                Path(classify_image_path).joinpath(dataset_name).joinpath(class_name).mkdir(parents=True)
            
            # 截取数据集
            dataset_num = int(len_images * dataset['percent'])
            dataset_images = class_images[:dataset_num]
            class_images = class_images[dataset_num:]
            
            for image in dataset_images:
                image_name = image.replace('.bmp', '')
                print(class_name, dataset_name, image_name)
                # 将标记过的图像文件复制到图像集文件夹中
                classify_copy_image(dataset_name, class_name, image_name)


def detect_train():
    print("====================detect_train====================")
    model = YOLO('yolov8n.yaml', task='detect')  # 从YAML中构建一个新模型
    # model = YOLO('yolov8n.pt')  #加载预训练的模型(推荐用于训练)
    # model = YOLO('yolov8n.yaml').load('yolov8n.pt')  # 从YAML构建并传递权重

    # Train the model
    model.train(data='detect.yaml', exist_ok=True, epochs=1000, batch=16)


def detect_validate():
    print("====================detect_validate====================")
    # model = YOLO('yolov8n.pt')  #加载官方模型
    model = YOLO('runs/detect/train/weights/best.pt')  # 加载自己训练的模型

    # Validate the model
    metrics = model.val(exist_ok=True)
    print(metrics)
    # metrics.box.map    # map50-95
    # metrics.box.map50  # map50
    # metrics.box.map75  # map75
    # metrics.box.maps   # a list contains map50-95 of each category


def detect_test():
    print("====================detect_test====================")
    model = YOLO('runs/detect/train/weights/best.pt')  # 加载自己训练的模型

    # Validate the model
    metrics = model.val(split='test', name='test', exist_ok=True)
    print(metrics)


def detect_predict():
    print("====================detect_predict====================")
    # for image in Path(detect_pred_image_path).iterdir():
    #     remove_file_or_folder(image)
    # model = YOLO('yolov8n.pt')  #加载官方模型
    model = YOLO('runs/detect/train/weights/best.pt')  # 加载自己训练的模型

    # Predict with the model
    for image_path in Path(orig_image_path).iterdir():        
        results = model.predict(source=image_path)  # predict on an image
        result = results[0]
        image_array = result.plot()
        image = Image.fromarray(image_array[..., ::-1])
        file_path = Path(detect_pred_image_path).joinpath(f'{image_path.name}')
        image.save(file_path)
    
    # 目标检测后处理
    # boxes = results[0].boxes
    # boxes.xyxy  # box with xyxy format, (N, 4)
    # boxes.xywh  # box with xywh format, (N, 4)
    # boxes.xyxyn  # box with xyxy format but normalized, (N, 4)
    # boxes.xywhn  # box with xywh format but normalized, (N, 4)
    # boxes.conf  # confidence score, (N, 1)
    # boxes.cls  # cls, (N, 1)
    # boxes.data  # raw bboxes tensor, (N, 6) or boxes.boxes .

    # 实例分割后处理
    # masks = results[0].masks  # Masks object
    # masks.segments  # bounding coordinates of masks, List[segment] * N
    # masks.data  # raw masks tensor, (N, H, W) or masks.masks 

    # 目标分类后处理
    # results = model(inputs)
    # results[0].probs  # cls prob, (num_class, )


def classify_train():
    print("====================classify_train====================")
    model = YOLO('yolov8n-cls.yaml', task='classify')  # 从YAML中构建一个新模型
    # Train the model
    # model.train(data='classify.yaml', epochs=3, batch=16)
    model.train(data='datasets/classify/images', exist_ok=True, epochs=1000, batch=16)


def classify_validate():
    print("====================classify_validate====================")
    model = YOLO('runs/classify/train/weights/best.pt')  # 加载自己训练的模型
    # Validate the model
    metrics = model.val(exist_ok=True)
    print(metrics)


def classify_test():
    print("====================classify_test====================")
    model = YOLO('runs/classify/train/weights/best.pt')  # 加载自己训练的模型

    # Validate the model
    metrics = model.val(split='test', name='test', exist_ok=True)
    print(metrics)


def classify_predict():
    print("====================classify_predict====================")
    model = YOLO('runs/classify/train/weights/best.pt')  # 加载自己训练的模型
    for image in Path(classify_pred_image_path).iterdir():
        remove_file_or_folder(image)
    # Predict with the model
    for image_path in Path(orig_image_path).iterdir():        
        results = model.predict(source=image_path)  # predict on an image
        result = results[0]
        image_array = result.plot()
        image = Image.fromarray(image_array[..., ::-1])
        class_name = result.names[result.probs.top1]
        file_path = Path(classify_pred_image_path).joinpath(f'{class_name}_{image_path.name}')
        image.save(file_path)


def run(commands):
    if 'detect' in commands or 'all' in commands:
        if 'prepare' in commands or 'full' in commands:
            detect_prepare_data()
        if 'train' in commands or 'full' in commands:
            detect_train()
        if 'validate' in commands or 'full' in commands:
            detect_validate()
        if 'test' in commands or 'full' in commands:
            detect_test()
        if 'predict' in commands or 'full' in commands:
            detect_predict()

    if 'classify' in commands or 'all' in commands:
        if 'rough' in commands:
            classify_rough_split_images()
        if 'prepare' in commands or 'full' in commands:
            classify_prepare_data()
        if 'train' in commands or 'full' in commands:
            classify_train()
        if 'validate' in commands or 'full' in commands:
            classify_validate()
        if 'test' in commands or 'full' in commands:
            classify_test()
        if 'predict' in commands or 'full' in commands:
            classify_predict()


if __name__ == '__main__':
    run(sys.argv)
