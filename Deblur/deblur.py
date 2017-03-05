import numpy as np
from scipy.ndimage.filters import convolve
from scipy.misc import imread, imsave
from PIL import Image 

image = imread("earth.jpg", flatten=True)
kernel = imread("kernel.jpg", flatten=True)
incorrect_kernel = imread("kernel_wrong.jpg", flatten=True)
# Pad image
#iml = np.zeros(np.multiply(np.shape(image),2))
#iml[:image.shape[0],:image.shape[1]] = image
#image = iml

# Pad kernel
l = np.zeros_like(image)
l[:kernel.shape[0],:kernel.shape[1]] = kernel
kernel = l

l = np.zeros_like(image)
l[:incorrect_kernel.shape[0],:incorrect_kernel.shape[1]] = incorrect_kernel
incorrect_kernel = l

# Normalize values (0,255)->(0,1)
kernel = kernel/np.max(kernel)
kernel = kernel/np.sum(np.abs(kernel))
image = image/np.max(image)
image_fft = (np.fft.fft2(image))
kernel_fft = (np.fft.fft2(kernel))
kernel_inverse = 1/kernel_fft
kernel_inverse = np.fft.ifft( kernel_inverse )
Image.fromarray( 255*(np.abs(kernel_inverse)/np.max(kernel_inverse)).real).convert('RGB').save('output_kernel_inverse.jpg')

# Blur image
blurred_array = np.multiply(image_fft, kernel_fft)
blurred_array_shift = np.fft.fftshift( blurred_array )
Image.fromarray( 255*(np.abs(blurred_array_shift)/np.max(blurred_array_shift)).real).convert('RGB').save('output_fft_blurred.jpg')
blurred_array = np.fft.ifft2(blurred_array)

# Add noise
noisy_blurred_array = np.add( blurred_array, (np.random.rand(*blurred_array.shape )-0.5)/5000 ) 

# Deblur process
noisy_deblurred_array = np.fft.fft2( noisy_blurred_array )
noisy_deblurred_array_fft = np.fft.fftshift( noisy_deblurred_array )
Image.fromarray( 255*(np.abs(noisy_deblurred_array_fft)/np.max(noisy_deblurred_array_fft)).real).convert('RGB').save('output_fft_deblurred.jpg')
noisy_deblurred_array = np.divide( noisy_deblurred_array, kernel_fft )
noisy_deblurred_array = np.fft.ifft2( noisy_deblurred_array )

# Deblur with incorrect kernel for comparisson
incorrect_kernel_fft = np.fft.fft2(incorrect_kernel)
incorrect_noisy_deblurred_array = np.divide( noisy_deblurred_array, incorrect_kernel_fft )
incorrect_noisy_deblurred_array = np.fft.ifft2( incorrect_noisy_deblurred_array )

# Convert arrays to images 
noisy_blurred_image = Image.fromarray( 255*(np.abs(noisy_blurred_array)/np.max(noisy_blurred_array)).real).convert('RGB')
image_fft_image = Image.fromarray((image_fft).real).convert('RGB')
kernel_fft_image = Image.fromarray((kernel_fft).real).convert('RGB')
noisy_deblurred_image = Image.fromarray( 255*(np.abs(noisy_deblurred_array)/np.max(noisy_deblurred_array.real))).convert('RGB')
incorrect_noisy_deblurred_image = Image.fromarray( 255*(np.abs(incorrect_noisy_deblurred_array)/np.max(incorrect_noisy_deblurred_array.real))).convert('RGB')

# Save images
incorrect_noisy_deblurred_image.save('output_incorrect_image_deblurred_noisy.jpg')
noisy_deblurred_image.save('output_image_deblurred_noisy.jpg')
noisy_blurred_image.save('output_image_blurred_noisy.jpg')
image_fft_image.save('output_image_fft.jpg')
kernel_fft_image.save('output_kernel_fft.jpg')

