import cv2
import os
from pathlib import Path

input_dir = "gaussian"
output_dir = "gaussian_padded"
padding = 1
border_type = cv2.BORDER_CONSTANT   
border_value = 0                    

os.makedirs(output_dir, exist_ok=True)

for filename in os.listdir(input_dir):
    input_path = os.path.join(input_dir, filename)

    # Skip non-files
    if not os.path.isfile(input_path):
        continue

    # Read image (keeps color or grayscale as-is)
    image = cv2.imread(input_path, cv2.IMREAD_UNCHANGED)

    if image is None:
        print(f"Skipping (not an image): {filename}")
        continue

    padded = cv2.copyMakeBorder(
        image,
        padding, padding, padding, padding,
        border_type,
        value=border_value
    )

    output_path = os.path.join(output_dir, filename)
    cv2.imwrite(output_path, padded)

    print(f"Padded: {filename}")