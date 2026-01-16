import nibabel as nib
import numpy as np
import matplotlib.pyplot as plt

# Charger un volume NIfTI
img = nib.load("test_ds.nii.gz")
data = img.get_fdata()

# Extraire une coupe axiale centrale
slice_idx = data.shape[2] // 2
slice_img = data[:, :, slice_idx]

# Normalisation
slice_img = (slice_img - slice_img.min()) / (slice_img.max() - slice_img.min())

# Sauvegarde PNG
plt.imsave("slice.png", slice_img, cmap="gray")
