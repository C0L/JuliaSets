import numpy as np
from PIL import Image

R = 1.5
X = 1000
Y = 1000
max_iter = 1000

grid = np.zeros((X,Y), dtype=complex)
img = np.zeros((X,Y), dtype = 'uint8')

def f(z):
  #return np.square(z) + (-0.8 + 0.156j)
  return np.square(z) +  -0.8j
# Initialize the grid
for i in range(X):
  for j in range(Y):
    # Make 0,0 the center of the grid
    ci = i-(X/2)
    cj = -(j-(Y/2))
    ai = (0 if ci == 0 else ((ci + (X/2)) * (2*R))/X - R)
    aj = (0 if cj == 0 else ((cj + (Y/2)) * (2*R))/Y - R)

    grid[i][j] = complex((ai), (aj))

for i in range(X):
  for j in range(Y):
    it = 0
    #while (np.abs(grid[i][j]) < R**2 and it < max_iter):
    while (grid[i][j].real*grid[i][j].real + grid[i][j].imag*grid[i][j].imag < R**2 and it < max_iter):
      grid[i][j] = f(grid[i][j]) 
      it += 1

    if (it == max_iter):
      print("MAX OUT")
      img[i][j] = 0
    else:
      img[i][j] = it % 256

print(img.mean())
ig = Image.fromarray(img.astype('uint8'), 'P')
ig.save('test.png')
