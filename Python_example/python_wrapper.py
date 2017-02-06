
import numpy
from numpy.ctypeslib import ndpointer
import scipy.misc
import ctypes
import sys

def numpycircle(image, cx, cy, radius, color, transparency=0.0):
    """Draw a circle on an image using only numpy methods."""
    radius = int(radius)
    cx = int(cx)
    cy = int(cy)
    y, x = numpy.ogrid[-radius: radius, -radius: radius]
    index = x**2 + y**2 <= radius**2
    image[cy-radius:cy+radius, cx-radius:cx+radius][index] = (
        image[cy-radius:cy+radius, cx-radius:cx+radius][index].astype('float32') * transparency +
        numpy.array(color).astype('float32') * (1.0 - transparency)).astype('uint8')


def mainfunc():
    
        ## see this how to send contiguous array:
        ##http://stackoverflow.com/questions/22615296/calling-c-from-python-with-ctypes-passing-vectors
        ##http://stackoverflow.com/questions/33813272/python-ctypes-pass-c-void-p-as-an-out-parameter-to-c-function
        ##http://stackoverflow.com/questions/10744875/ctypes-passing-string-by-reference
        ##http://stackoverflow.com/questions/4355524/getting-data-from-ctypes-array-into-numpy
        
        lib = ctypes.cdll.LoadLibrary('./libdlib_face.so')
        fun = lib.face_rects
        fun.argtypes = [ndpointer(ctypes.c_int),ctypes.c_int,ctypes.c_int,ctypes.c_int,ctypes.c_int,ctypes.c_int,ctypes.POINTER(ctypes.POINTER(ctypes.c_int))]
        fun.restype = ctypes.py_object

        
        
        image = scipy.misc.imread("white_dress.jpg",mode='RGB') #(3x8-bit pixels, true color)
        
        
        ## Don't use this! Even with numpy.ascontiguousarray() this will not return appropriate data!
        ## fun(ctypes.c_void_p(image.ctypes.data), saveshape[0],saveshape[1], 8, 1, 0,ctypes.c_void_p(outdata.ctypes.data))
    
        pointer_ar = ctypes.POINTER(ctypes.c_int)() # passing pointe empty array by reference. Double pointer structure allows to pass pointer from c++
        
        to_pyr_up = 1;
        
        numres = fun(numpy.ascontiguousarray(image,numpy.int32), image.shape[0],image.shape[1], 8, image.shape[2], to_pyr_up, ctypes.byref(pointer_ar))
        
        
        
        # print(numres)
        
        # restoring array from pointer and length
        array_length = numres[0]*4
        
        if (array_length != 0):
                ## throws errors if dlib found nothing and array is empty
                arr = numpy.ctypeslib.as_array( (ctypes.c_int * array_length).from_address(ctypes.addressof(pointer_ar.contents)))
                
                arr = arr/(to_pyr_up+1)
                
                # Python 2.7 solution
                #buffer = numpy.core.multiarray.int_asbuffer(ctypes.addressof(pointer_ar.contents), 4*array_length)
                #arr = numpy.frombuffer(buffer, dtype=numpy.int32)               
                print(arr)
                
                
                numpycircle(image,
                        (arr[0]+arr[2])/2,
                        (arr[1]+arr[3])/2,
                        max((arr[2]-arr[0]),(arr[3]-arr[1]))/2,
                        [255,0,255],
                        0.5)
                scipy.misc.imsave('out.png', image)
                
                
        else:
                print("No face found. Try PyrUp more")
       

# main function
if __name__ == "__main__":
        mainfunc()