#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <string>

#include <dlib/image_io.h>
#include "../dlib_image_from_array.h"

/*

OpenCV Mat element types and their sizes

http://stackoverflow.com/questions/15245262/opencv-mat-element-types-and-their-sizes

8-bit unsigned integer (uchar) 
8-bit signed integer (schar)
16-bit unsigned integer (ushort)
16-bit signed integer (short)
32-bit signed integer (int)
32-bit floating-point number (float)
64-bit floating-point number (double)
...

For these basic types, the following enumeration is applied:
enum { CV_8U=0, CV_8S=1, CV_16U=2, CV_16S=3, CV_32S=4, CV_32F=5, CV_64F=6 };
*/

int main(){
    
    cv::Mat mat; 
    
    
    // ----------------------------------------------------------------------------------------
    
    // read image in opencv as 8-bit grayscale
    mat = cv::imread("white_dress.jpg", CV_LOAD_IMAGE_GRAYSCALE); 
    
    //cv::imwrite("out_cv.png",mat);
    
    // convert cv::Mat to contiguous 1-D array
    // watch out for indexing!
    int *array = new int[mat.rows*mat.cols+1];
    for (int i=1; i<=mat.rows; ++i) {
        for (int j=1; j<=mat.cols; ++j) {
           array[mat.cols*(i-1)+j] = mat.at<uchar>(i-1,j-1);
        }
    }

    // send to dlib
    // return_matrix_of_rgb_pixel(int* original_img, unsigned height_, unsigned width_, unsigned bit_depth_, unsigned channels, bool bgr = 0);
    // we can use rgb_pixel there, another variant with uint16 below
    matrix<rgb_pixel> dlib_img = return_matrix_of_rgb_pixel(&array[1], mat.rows, mat.cols, 8, 1, 1);
    
    // save image in dlib
    save_png(dlib_img, "out_GR8.png");
    
    
    // ----------------------------------------------------------------------------------------
    
    // pass image as 16-bit grayscale (opencv does not support direct conversion)
    mat = cv::imread("white_dress.jpg");
    cv::cvtColor(mat, mat, CV_BGR2GRAY);
    cv::Mat pic16bit;
    mat.convertTo(pic16bit, CV_16U, 255); //convert to 16-bit by multiplying all values by 255, CV_16U corresponds to 16 bit unsigned integers, meaning your range is [0; 255 * 255].
    
    delete[] array;
    // convert cv::Mat to contiguous 1-D array
    // watch out for indexing!
    array = new int[pic16bit.rows*pic16bit.cols+1];
    for (int i=1; i<=pic16bit.rows; ++i) {
        for (int j=1; j<=pic16bit.cols; ++j) {
           array[pic16bit.cols*(i-1)+j] = pic16bit.at<ushort>(i-1,j-1);
        }
    }
    
    // send to dlib
    matrix<dlib::uint16> dlib_img_g16 = return_matrix_of_gray16_pixel(&array[1], mat.rows, mat.cols, 16, 1, 1);
    
    // save image in dlib
    save_png(dlib_img_g16, "out_GR16_from_GR8.png");
    
    
    // ----------------------------------------------------------------------------------------
    
    //pass image as 8-bit grayscale with alpha = 0.5
    
    mat = cv::imread("white_dress.jpg", CV_LOAD_IMAGE_GRAYSCALE); 
    
    delete[] array;
    // convert cv::Mat to contiguous 1-D array
    // watch out for indexing!
    array = new int[(mat.rows*mat.cols+1)*2];
    for (int i=1; i<=mat.rows; ++i) {
        for (int j=1; j<=mat.cols; ++j) {
           array[(mat.cols*(i-1)+j)*2] = mat.at<uchar>(i-1,j-1);
           array[(mat.cols*(i-1)+j)*2+1] = (uchar)(0.5*255); //setting 8-bit alpha channel, 0..255
           
        }
    }
    // send to dlib
    // watch out: channels = 2 !
    matrix<rgb_alpha_pixel> dlib_img_rgba = return_matrix_of_rgb_alpha_pixel(&array[2], mat.rows, mat.cols, 8, 2, 1);
    
    // save image in dlib
    save_png(dlib_img_rgba, "out_GR8_alpha.png");
    
    
    // ----------------------------------------------------------------------------------------
    
    //pass image as 16-bit grayscale with alpha = 0.5
    
    mat = cv::imread("white_dress.jpg"); 
    
    cv::cvtColor(mat, mat, CV_BGR2GRAY);
    //cv::Mat pic16bit;
    pic16bit.release();
    mat.convertTo(pic16bit, CV_16U, 255); //convert to 16-bit by multiplying all values by 255, CV_16U corresponds to 16 bit unsigned integers, meaning your range is [0; 255 * 255].
    
    delete[] array;
    // convert cv::Mat to contiguous 1-D array
    // watch out for indexing!
    array = new int[(pic16bit.rows*pic16bit.cols+1)*2];
    for (int i=1; i<=pic16bit.rows; ++i) {
        for (int j=1; j<=pic16bit.cols; ++j) {
           array[(pic16bit.cols*(i-1)+j)*2] = pic16bit.at<ushort>(i-1,j-1);
           array[(pic16bit.cols*(i-1)+j)*2+1] = (ushort)(0.5*255*255); //setting 8-bit alpha channel, 0..255. Int is 4 bytes = 32 bit, so we still can convert without loss
           
        }
    }
    
    // send to dlib
    dlib_img_rgba = return_matrix_of_rgb_alpha_pixel(&array[2], mat.rows, mat.cols, 16, 2, 1);
    
    // save image in dlib
    save_png(dlib_img_rgba, "out_GR16_from_GR8_alpha.png");
    
    
    // ----------------------------------------------------------------------------------------
    
    //pass image as 8-bit rgb
    
    mat = cv::imread("white_dress.jpg"); 
    
    delete[] array;
    // convert cv::Mat to contiguous 1-D array
    // watch out for indexing!
    array = new int[(mat.rows*mat.cols+1)*3];
    for (int i=1; i<=mat.rows; ++i) {
        for (int j=1; j<=mat.cols; ++j) {
            
           cv::Vec3b pixelval = mat.at<cv::Vec3b>(i-1,j-1); //CV_8UC3
           
           array[(mat.cols*(i-1)+j)*3] = pixelval[0];
           array[(mat.cols*(i-1)+j)*3+1] = pixelval[1];
           array[(mat.cols*(i-1)+j)*3+2] = pixelval[2];
        }
    }
    
    
    // send to dlib
    // channels = 3 and BGR = 1 !
    dlib_img = return_matrix_of_rgb_pixel(&array[3], mat.rows, mat.cols, 8, 3, 1);
    
    // save image in dlib
    save_png(dlib_img, "out_RGB8.png");
    
    
    // ----------------------------------------------------------------------------------------
    
    //pass image as 16-bit rgb
    
    mat = cv::imread("white_dress.jpg"); 
    
    pic16bit.release();
    mat.convertTo(pic16bit, CV_16UC3, 255); //convert to 16-bit by multiplying all values by 255
    
    delete[] array;
    // convert cv::Mat to contiguous 1-D array
    // watch out for indexing!
    array = new int[(pic16bit.rows*pic16bit.cols+1)*3];
    for (int i=1; i<=pic16bit.rows; ++i) {
        for (int j=1; j<=pic16bit.cols; ++j) {
           
           cv::Vec3w pixelval = pic16bit.at<cv::Vec3w>(i-1,j-1);//CV_16UC3
           
           array[(pic16bit.cols*(i-1)+j)*3] = pixelval[0];
           array[(pic16bit.cols*(i-1)+j)*3+1] = pixelval[1];
           array[(pic16bit.cols*(i-1)+j)*3+2] = pixelval[2];
           
           
        }
    }
    
    // send to dlib
    // channels = 3 and BGR = 1 !
    dlib_img = return_matrix_of_rgb_pixel(&array[3], mat.rows, mat.cols, 16, 3, 1);
    
    // save image in dlib
    save_png(dlib_img, "out_RGB16.png");
    
    
    // ----------------------------------------------------------------------------------------
    
    //pass image as 8-bit rgba
    mat = cv::imread("white_dress.jpg"); 
    
    delete[] array;
    // convert cv::Mat to contiguous 1-D array
    // watch out for indexing!
    array = new int[(mat.rows*mat.cols+1)*4];
    for (int i=1; i<=mat.rows; ++i) {
        for (int j=1; j<=mat.cols; ++j) {
            
           cv::Vec3b pixelval = mat.at<cv::Vec3b>(i-1,j-1); //CV_8UC3
           
           array[(mat.cols*(i-1)+j)*4] = pixelval[0];
           array[(mat.cols*(i-1)+j)*4+1] = pixelval[1];
           array[(mat.cols*(i-1)+j)*4+2] = pixelval[2];
           
           array[(mat.cols*(i-1)+j)*4+3] = (uchar)(0.5*255); //setting 8-bit alpha channel, 0..255
        }
    }
    
    // send to dlib
    // watch out: channels = 4 !
    dlib_img_rgba = return_matrix_of_rgb_alpha_pixel(&array[4], mat.rows, mat.cols, 8, 4, 1);
    
    // save image in dlib
    save_png(dlib_img_rgba, "out_RGBA8.png");
    
    // ----------------------------------------------------------------------------------------
    
    //pass image as 16-bit rgba
    mat = cv::imread("white_dress.jpg"); 
    
    pic16bit.release();
    mat.convertTo(pic16bit, CV_16UC3, 255); //convert to 16-bit by multiplying all values by 255
    
    delete[] array;
    
    // convert cv::Mat to contiguous 1-D array
    // watch out for indexing!
    array = new int[(pic16bit.rows*pic16bit.cols+1)*4];
    for (int i=1; i<=pic16bit.rows; ++i) {
        for (int j=1; j<=pic16bit.cols; ++j) {
           
           cv::Vec3w pixelval = pic16bit.at<cv::Vec3w>(i-1,j-1);//CV_16UC3
           
           array[(pic16bit.cols*(i-1)+j)*4] = pixelval[0];
           array[(pic16bit.cols*(i-1)+j)*4+1] = pixelval[1];
           array[(pic16bit.cols*(i-1)+j)*4+2] = pixelval[2];
           
           array[(pic16bit.cols*(i-1)+j)*4+3] = (ushort)(0.5*255*255); //setting 16-bit alpha channel, 0..255*255
           
        }
    }
    
    // send to dlib
    // channels = 4 !
    dlib_img_rgba = return_matrix_of_rgb_alpha_pixel(&array[4], mat.rows, mat.cols, 16, 4, 1);
    
    // save image in dlib
    save_png(dlib_img_rgba, "out_RGBA16.png");
    
}
