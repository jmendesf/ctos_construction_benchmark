import csv
import os
import requests
from io import BytesIO
from PIL import Image

INPUT_FILE = "photos.csv000"
OUT_DIR = "unsplash_png2"
MAX_IMAGES = 2000         
TIMEOUT_S = 20

os.makedirs(OUT_DIR, exist_ok=True)

def download_and_convert(url: str, out_path: str):
    r = requests.get(url, timeout=TIMEOUT_S)
    r.raise_for_status()
    img = Image.open(BytesIO(r.content)).convert("RGB")
    img.save(out_path, format="PNG", optimize=True)

count = 0
with open(INPUT_FILE, "r", encoding="utf-8", newline="") as f:
    reader = csv.DictReader(f, delimiter="\t")   # tab-separated
    for row in reader:
        if count >= MAX_IMAGES:
            break

        url = row.get("photo_image_url", "")
        if not url:
            continue

        out_path = os.path.join(OUT_DIR, f"unsplash_{count:06d}.png")

        try:
            download_and_convert(url, out_path)
            count += 1
            if count % 50 == 0:
                print(f"Downloaded {count}")
        except Exception as e:
            print("Skip:", url, "->", e)

print("Done. PNGs:", count)
