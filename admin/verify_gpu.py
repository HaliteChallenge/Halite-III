"""
Simple script to check if GPUs are working.
"""

# Interestingly, importing torch first tends to make Python segfault.
import tensorflow
import torch

assert torch.cuda.is_available(), "pytorch is not available"

from tensorflow.python.client import device_lib
print(device_lib.list_local_devices())
print("A GPU device should appear.")
