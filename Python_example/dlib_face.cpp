// Example usage of this dlib wrapper for Python
// Here you need only face_rects() function, everything else belongs to dlib NN face recognition taken from here http://dlib.net/dnn_mmod_face_detection_ex.cpp.html


#include <Python.h> 
//Since Python may define some pre-processor definitions which affect the standard headers on some systems, you must include Python.h before any standard headers are included.

#include <iostream>
#include <dlib/dnn.h>
#include <dlib/image_processing.h>
#include "../dlib_image_from_array.h"

#include <vector>
#include <algorithm>


//using namespace std;
using namespace dlib;

// ----------------------------------------------------------------------------------------

template <long num_filters, typename SUBNET> using con5d = con<num_filters,5,5,2,2,SUBNET>;
template <long num_filters, typename SUBNET> using con5  = con<num_filters,5,5,1,1,SUBNET>;

template <typename SUBNET> using downsampler  = relu<affine<con5d<32, relu<affine<con5d<32, relu<affine<con5d<16,SUBNET>>>>>>>>>;
template <typename SUBNET> using rcon5  = relu<affine<con5<45,SUBNET>>>;

using net_type = loss_mmod<con<1,9,9,1,1,rcon5<rcon5<rcon5<downsampler<input_rgb_image_pyramid<pyramid_down<6>>>>>>>>;

net_type net;

matrix<rgb_pixel> img;

int initialized = 0;

int return_face_rects(matrix<rgb_pixel> &img, int** &results, int to_pyramid_up); // prototype

void init_dlib(){  // Initializer to set variables needed. Can be more complicated
    
    deserialize("mmod_human_face_detector.dat") >> net; 
    
    initialized = 1;
    
}


// ----------------------------------------------------------------------------------------
// Function to send and receive Python arrays
// http://stackoverflow.com/questions/5862915/passing-numpy-arrays-to-a-c-function-for-input-and-output

extern "C"
{

PyObject* face_rects(const void * indatav, unsigned rowcount, unsigned colcount, unsigned depth, unsigned channels, int to_pyramid_up,int ** outdata) {
    
    int * indata = (int *) indatav;
     
   
    //receiving matrix from python
    img = return_matrix_of_rgb_pixel(indata, rowcount, colcount,depth, channels, 0);
    
    int** results; //array can be returned to Python
    
    // computing results 
    int numres = return_face_rects(img, results, to_pyramid_up);
    
    // preparing to send back to python
    
            /*
    // creating contiguous array - don't need in this example, as contiguous array returned from function below by design
             //create 1D array
             int rows = numres;
             int cols = 4;
             int *res1D = new int[rows*cols];
             //copy contents
             int *targetPointer = res1D;
             int **sourcePointer = results;
             int *endTargetPointer = targetPointer + rows*cols;
             while( targetPointer != endTargetPointer) {        
                 std::copy_n(*sourcePointer++, cols, targetPointer );
                 targetPointer += cols;
             }
             */
    
    
    //resulting contiguous array is res1D above;
    
    // sending pointer to array (it is contiguous!)
    // It is passed by reference in python, so this assignment is ok
    // Python will care about memory further on
    *outdata = &results[0][0]; // = res1D
    
    //returning list that consists of some other data (array length)
    PyObject* result = PyList_New(0);
    
    PyList_Append(result, PyLong_FromLong(numres));
    
    
    return result;
}


}// extern "C"



// actual working function
int return_face_rects(matrix<rgb_pixel> &img, int** &results, int to_pyramid_up = 1){ //function returns data length and pointer do array. If = 0 then nothing found.
    
    
    if(!initialized) {init_dlib();} //to make user not to care about manual initialization. Can be in the Python mudule init part, but let this one check be there
    
    
    //load_image(img, imagepath);
    
    
    int loopcounter; //one loopcounter to be used
    
    if (to_pyramid_up) { //if to_pyramid_up != 0 then we do as many iterations as was set there
        
        for(loopcounter=0;loopcounter<to_pyramid_up;loopcounter++) {
            
            pyramid_up(img);
            
        }
        
    } 
    
    
    
     // Note that you can process a bunch of images in a std::vector at once and it runs
        // much faster, since this will form mini-batches of images and therefore get
        // better parallelism out of your GPU hardware.  However, all the images must be
        // the same size.  To avoid this requirement on images being the same size we
        // process them individually in this example.
    //auto dets = net(img);
    
    
    std::vector<dlib::mmod_rect> dets = net(img); //this is a vector and we know its length
    
    
    /*
     struct mmod_rect
    {
        
      //      WHAT THIS OBJECT REPRESENTS
      //          This is a simple struct that is used to give training data and receive detections
      //          from the Max-Margin Object Detection loss layer loss_mmod_ object.
        

        mmod_rect() = default; 
        mmod_rect(const rectangle& r) : rect(r) {}
        mmod_rect(const rectangle& r, double score) : rect(r),detection_confidence(score) {}

        rectangle rect;
        double detection_confidence = 0;
        bool ignore = false;

        operator rectangle() const { return rect; }
    };
    */
    
    
    
    loopcounter = 0;
    
    //allocating results array in contiguous memory block for convenience
    //http://stackoverflow.com/questions/41371204/fast-convert-double-pointer-array-into-single-pointer-probably-can-be-continuou
    int rows = dets.size();
    int cols = 4;
    // Allocate pointers:
    int** outs = new int*[rows];
    // Allocate data:
    auto ptr=new int[rows*cols];
    
    for(loopcounter = 0; loopcounter < rows; loopcounter++) {
        outs[loopcounter] = &ptr[loopcounter*cols];
    }
    
    loopcounter = 0;
    
    for (dlib::mmod_rect& r : dets) { //r seems to be dlib::mmod_rect
        
        int* rect = outs[loopcounter];
        
        rect[0] = r.rect.left();
        rect[1] = r.rect.top();
        rect[2] = r.rect.right() + 1;
        rect[3] = r.rect.bottom() + 1;
        
        //dlib:: rectangle class also seems to have 
        //unsigned long w,
        //unsigned long h
        
        loopcounter++;
        
    } 
    
    results = outs;
    
    
    return loopcounter;
}



//Original Main Func

/*

int main(int argc, char** argv) try
{
    if (argc == 1)
    {
        cout << "Call this program like this:" << endl;
        cout << "./dnn_mmod_face_detection_ex mmod_human_face_detector.dat faces/*.jpg" << endl;
        cout << "\nYou can get the mmod_human_face_detector.dat file from:\n";
        cout << "http://dlib.net/files/mmod_human_face_detector.dat.bz2" << endl;
        return 0;
    }


    net_type net;
    deserialize(argv[1]) >> net;  

    image_window win;
    for (int i = 2; i < argc; ++i)
    {
        matrix<rgb_pixel> img;
        load_image(img, argv[i]);

        // Upsampling the image will allow us to detect smaller faces but will cause the
        // program to use more RAM and run longer.
        //while(img.size() < 1800*1800)
            pyramid_up(img);  // just one upsample is enough for our purpose

        // Note that you can process a bunch of images in a std::vector at once and it runs
        // much faster, since this will form mini-batches of images and therefore get
        // better parallelism out of your GPU hardware.  However, all the images must be
        // the same size.  To avoid this requirement on images being the same size we
        // process them individually in this example.
        auto dets = net(img);
        win.clear_overlay();
        win.set_image(img);
        for (auto&& d : dets)
            win.add_overlay(d);

        cout << "Hit enter to process the next image." << endl;
        cin.get();
    }
}
catch(std::exception& e)
{
    cout << e.what() << endl;
}


*/
 
