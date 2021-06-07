'''
Tool for processing binary jumps from juliaset.c
'''
from PIL import Image
import numpy as np
import glob
import sys
from natsort import natsorted
from matplotlib import cm

input_file = glob.glob(sys.argv[1])
input_size = int(sys.argv[2])

for f in natsorted(input_file):
  print(f)
  dat = np.fromfile(f, dtype = 'uint8')
  dat = dat.reshape((input_size,input_size))
  ig = Image.fromarray(np.uint8(cm.inferno(dat)*255))
  ig.save(f + '.png')
