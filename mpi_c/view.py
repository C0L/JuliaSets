from PIL import Image
import numpy as np
from matplotlib import cm

dat = np.fromfile('test.dat', dtype = 'uint8')
dat = dat.reshape((10000,10000))
#print(dat.mean())
#x = dat[0:1000][0:1000]
#print(type(x))
#x = x.reshape((1000,1000))
#.reshape((1000,1000))
#ig = Image.fromarray(x.astype('uint8')).convert('P', palette = Image.WEB)
#ig = Image.fromarray(np.uint8(cm.viridis(dat)*255))
ig = Image.fromarray(np.uint8(cm.inferno(dat)*255))
ig.save('test.png')
