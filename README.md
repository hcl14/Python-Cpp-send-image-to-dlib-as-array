# Send image from C++ and Python to dlib

As long as dlib NN api is not supported in Python, I wrote a simple python and C++ wrapper to send image into dlib C++ code from arbitrary container (I have `int*` for Python)

## Getting Started

`dlib_image_from_array.h` does not need any dependency except dlib. Examples in the corresponding subfolders show usage in C++ and Python, I used OpenCV to quickly create different matrices. The code accepts `int *` data along with dimension, depth and channel data, but you can easily use any peculiar container by modifying variable types and `navigate_in_array()` function. <br />

The code is based on Dlib's `png_loader.h` and does not support 16-bit image data except non-alpha grayscale. 16-bit pixel values are converted to 8-bit during input, see https://github.com/davisking/dlib/issues/428

### Prerequisites and Installing

Use cmake scripts to compile. Compile shared library (.so) to import into Python module. Don't forget to add `cmake . -DCMAKE_BUILD_TYPE=Release` or dlib will work slow.<b /> 

Tested on Dlib 19, OpenCV3, Python 2.7 and Python 3.5.


Upd: Recently I found this question http://stackoverflow.com/questions/41991651/how-to-assign-memory-images-to-a-dlib-array2d-or-image

so may be I will try to make better implementation sometime, based on these hints.
