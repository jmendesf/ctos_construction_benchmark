import sys
import os
from pathlib import Path
import cv2
import torch
import torchvision.transforms as transforms
from PIL import Image

if len(sys.argv) != 3:
    print('Please specify an input and output folder.')
    exit(0)

input_dir = sys.argv[1]
output_dir = sys.argv[2]

padding = 1
border_type = cv2.BORDER_CONSTANT   
border_value = 0     

nb_images_generated = 0
nb_images_treated = 0

os.makedirs(output_dir, exist_ok=True)

for filename in os.listdir(input_dir):
    input_path = os.path.join(input_dir, filename)

    # Skip non-files
    if not os.path.isfile(input_path):
        continue

    name, file_extension = os.path.splitext(input_path)
    if file_extension != '.jpeg' and file_extension != '.jpg' and file_extension != '.png':
        print(f"Skipping (not an image): {filename}")
        continue

    print('Opening ' + input_path)
    image = Image.open(input_path).convert('L')

    width, height = image.size

    if width <= height:
        min_dim = width
    else:
        min_dim = height
    
    crop_size = 50
    while crop_size * 2 < min_dim:
        transform = transforms.RandomCrop(crop_size)
        image_crop = transform(image)
        width, height = image_crop.size
        result = Image.new(image_crop.mode, (width + padding * 2, height + padding * 2), 0)
        result.paste(image_crop, (padding, padding))

        os.makedirs(output_dir + '/crop' + str(crop_size), exist_ok=True)

        output_path = os.path.join(output_dir + '/crop' + str(crop_size) + '/', filename)
        result.save(output_path)
        nb_images_generated += 1
        crop_size += 50
    print(f"Treated: {filename}")
    nb_images_treated += 1
print('---------------')
print('Treated ', nb_images_treated, ' images.')
print('Generated ', nb_images_generated, ' images')