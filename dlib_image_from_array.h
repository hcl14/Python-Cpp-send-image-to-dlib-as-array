#pragma once

// This function is a modification of the code of dlib png loader: http://dlib.net/dlib/image_loader/png_loader.h.html
// I've left original code commented out to make comparison easier

// PNG save code http://dlib.net/dlib/external/libpng/pngwutil.c.html supports more formats, line 16-bit 4-channels, you may try to add these types if you can find in dlib souce how they are processed
// This code ( and original dlib load_png() ) does not seem to support 16-bit, so all input 16-bit array data is converted to 8-bit (except non-alpha grayscale one, it seems to work)

// It accepts image is single one-dimensional contiguous integer array, but you can easily modify it for your data type,
// Just replace 'int* original_img' with your type and rewrite function navigate_in_array() to return the pointer to the array[4] which contains integer r,g,b,a pixel data

#include <vector>
#include <dlib/image_loader/png_loader.h>

using namespace dlib;


// ----------------------------------------------------------------------------------------

// i <-> rows, j <-> cols

int* navigate_in_array(unsigned& i, unsigned& j, int* array, unsigned& rows, unsigned& cols, unsigned& channels){ //do not copy anything to save time
    
    //return pointer to the pixel at img[i][j]
    //row and column indexing starts from 0 here!
    return &array[(cols*i+j)*channels];
}


//Different types of input using template function, just like in http://dlib.net/dlib/image_loader/png_loader.h.html
template <typename image_type>

//original_img is integer array of length RowsxColsxPixel_array_length 

// One simplification: if imported image has alpha channel, then it is present for all pixels 
// BGR is needed for OpenCV which follows its rules in Python as well, so beware when you will receive arrays with Python cv2 image data
// Or you can play with Dlib "bgr_pixel" type

void load_png_to_dlib_img(int* original_img, unsigned height_, unsigned width_, unsigned bit_depth_, unsigned channels, image_type& image, bool bgr = 0){
    
    // porting the code
    bool is_gray = (channels == 1);
    bool is_graya = (channels == 2);
    bool is_rgb = (channels == 3);
    bool is_rgba = (channels == 4);
    
    
    image_view<image_type> t(image);
    
    //setting dimensions
    t.set_size( height_, width_ );
    
    
    
            if (is_gray && bit_depth_ == 8)
            {
                        
                unsigned char p; //image element, there is only one in non-alpha gray case
                
                for ( unsigned n = 0; n < height_;n++ )
                {
                    //const unsigned char* v = get_row( n );
                    for ( unsigned m = 0; m < width_;m++ )
                    {
                        //unsigned char p = v[m];
                        unsigned char p = static_cast<unsigned char> (*navigate_in_array(n,m,original_img,height_, width_, channels)); //casting image element to the specified type
                        
                        assign_pixel( t[n][m], p );
                    }
                }
            }
            else if (is_gray && bit_depth_ == 16)
            {
                
                
                dlib::uint16 p;
                for ( unsigned n = 0; n < height_;n++ )
                {
                    //const uint16* v = (uint16*)get_row( n );
                    for ( unsigned m = 0; m < width_;m++ )
                    {
                        //dlib::uint16 p = v[m];
                        p = static_cast<dlib::uint16> (*navigate_in_array(n,m,original_img,height_, width_, channels)); //casting image element to the specified type
                        assign_pixel( t[n][m], p );
                    }
                }
            }
            else if (is_graya && bit_depth_ == 8) //here we have two values to read
            {
                
                
                for ( unsigned n = 0; n < height_;n++ )
                {
                    //const unsigned char* v = get_row( n );
                    for ( unsigned m = 0; m < width_; m++ )
                    {
                        /*
                        
                        unsigned char p = v[m*2];
                        if (!pixel_traits<pixel_type>::has_alpha)
                        {
                            assign_pixel( t[n][m], p );
                        }
                        else
                        {
                            unsigned char pa = v[m*2+1];
                            rgb_alpha_pixel pix;
                            assign_pixel(pix, p);
                            assign_pixel(pix.alpha, pa);
                            assign_pixel(t[n][m], pix);
                        }
                        */
                        
                        
                        int* addr = navigate_in_array(n,m,original_img,height_, width_, channels);
                        
                        rgb_alpha_pixel pix;
                        assign_pixel(pix, static_cast<unsigned char>(addr[0]));
                        assign_pixel(pix.alpha, static_cast<unsigned char>(addr[1])); //unsafe thing! But we hope user gave us correct dimensions
                        assign_pixel(t[n][m], pix);
                        
                    }
                }
            }
            else if (is_graya && bit_depth_ == 16)
            {                
                for ( unsigned n = 0; n < height_;n++ )
                {
                    //const uint16* v = (uint16*)get_row( n );
                    for ( unsigned m = 0; m < width_; m++ )
                    {
                        
                        // This their code should not work:
                        // according to http://dlib.net/dlib/pixel.h.html#rgb_alpha_pixel
                        // rgb_alpha_pixel is
                        
                        /*
                         unsigned char red;
                         unsigned char green;
                         unsigned char blue;
                         unsigned char alpha;
                         */
                        
                        //Their code:
                        
                        /*
                        dlib::uint16 p = v[m*2];
                        if (!pixel_traits<pixel_type>::has_alpha)
                        {
                            assign_pixel( t[n][m], p );
                        }
                        else
                        {
                            dlib::uint16 pa = v[m*2+1];
                            rgb_alpha_pixel pix;
                            assign_pixel(pix, p);
                            assign_pixel(pix.alpha, pa);
                            assign_pixel(t[n][m], pix);
                        }
                        */
                        int* addr = navigate_in_array(n,m,original_img,height_, width_, channels);
                        
                        // converting to 8-bit
                        rgb_alpha_pixel pix;
                        assign_pixel(pix,static_cast<unsigned char>(addr[0]/255.0));
                        assign_pixel(pix.alpha, static_cast<unsigned char>(addr[1]/255.0)); //unsafe thing! But we hope user gave us correct dimensions
                        assign_pixel(t[n][m], pix);
                        
                       
                        
                    }
                }
            }
            else if (is_rgb && bit_depth_ == 8)
            {
                for ( unsigned n = 0; n < height_;n++ )
                {
                    //const unsigned char* v = get_row( n );
                    for ( unsigned m = 0; m < width_;m++ )
                    {
                        /*
                        rgb_pixel p;
                        p.red = v[m*3];
                        p.green = v[m*3+1];
                        p.blue = v[m*3+2];
                        assign_pixel( t[n][m], p );
                        */
                        int* addr = navigate_in_array(n,m,original_img,height_, width_, channels);
                        
                        rgb_pixel p;
                        
                        if (!bgr){
                            p.red = static_cast<unsigned char>(addr[0]);
                            p.green = static_cast<unsigned char>(addr[1]);
                            p.blue = static_cast<unsigned char>(addr[2]);
                        } else {
                            p.blue = static_cast<unsigned char>(addr[0]);
                            p.green = static_cast<unsigned char>(addr[1]);
                            p.red = static_cast<unsigned char>(addr[2]);
                        }
                        
                        
                        assign_pixel( t[n][m], p );
                    }
                }
            }
            else if (is_rgb && bit_depth_ == 16)
            {
                for ( unsigned n = 0; n < height_;n++ )
                {
                    //const uint16* v = (uint16*)get_row( n );
                    for ( unsigned m = 0; m < width_;m++ )
                    {
                        
                        /*
                        rgb_pixel p;
                        p.red   = static_cast<uint8>(v[m*3]);
                        p.green = static_cast<uint8>(v[m*3+1]);
                        p.blue  = static_cast<uint8>(v[m*3+2]);
                        assign_pixel( t[n][m], p );
                        */
                        int* addr = navigate_in_array(n,m,original_img,height_, width_, channels);
                        
                        rgb_pixel p;
                        
                        // yep, still 8 there. Dividing by 255 to bring to 8-bit
                        if (!bgr){
                            p.red = static_cast<uint8>(addr[0]/255.0);
                            p.green = static_cast<uint8>(addr[1]/255.0);
                            p.blue = static_cast<uint8>(addr[2]/255.0);
                        } else {
                            p.blue = static_cast<uint8>(addr[0]/255.0);
                            p.green = static_cast<uint8>(addr[1]/255.0);
                            p.red = static_cast<uint8>(addr[2]/255.0);
                        }
                        
                        assign_pixel( t[n][m], p );
                    }
                }
            }
            else if (is_rgba && bit_depth_ == 8)
            {
                /*
                if (!pixel_traits<pixel_type>::has_alpha)
                    assign_all_pixels(t,0);
                */
                
                for ( unsigned n = 0; n < height_;n++ )
                {
                    //const unsigned char* v = get_row( n );
                    for ( unsigned m = 0; m < width_;m++ )
                    {
                        /*
                        rgb_alpha_pixel p;
                        p.red = v[m*4];
                        p.green = v[m*4+1];
                        p.blue = v[m*4+2];
                        p.alpha = v[m*4+3];
                        assign_pixel( t[n][m], p );
                        */
                        
                        int* addr = navigate_in_array(n,m,original_img,height_, width_, channels);
                        
                        rgb_alpha_pixel p;
                        
                        if (!bgr){
                            p.red = static_cast<unsigned char>(addr[0]);
                            p.green = static_cast<unsigned char>(addr[1]);
                            p.blue = static_cast<unsigned char>(addr[2]);
                            p.alpha = static_cast<unsigned char>(addr[3]);
                        } else {
                            p.blue = static_cast<unsigned char>(addr[0]);
                            p.green = static_cast<unsigned char>(addr[1]);
                            p.red = static_cast<unsigned char>(addr[2]);
                            p.alpha = static_cast<unsigned char>(addr[3]);
                        }
                        
                        assign_pixel( t[n][m], p );
                    }
                }
            }
            else if (is_rgba && bit_depth_ == 16)
            {
                /*
                if (!pixel_traits<pixel_type>::has_alpha)
                    assign_all_pixels(t,0);
                */

                for ( unsigned n = 0; n < height_;n++ )
                {
                    //const uint16* v = (uint16*)get_row( n );
                    for ( unsigned m = 0; m < width_;m++ )
                    {
                        /*
                        rgb_alpha_pixel p;
                        p.red   = static_cast<uint8>(v[m*4]);
                        p.green = static_cast<uint8>(v[m*4+1]);
                        p.blue  = static_cast<uint8>(v[m*4+2]);
                        p.alpha = static_cast<uint8>(v[m*4+3]);
                        assign_pixel( t[n][m], p );
                        */
                        
                        int* addr = navigate_in_array(n,m,original_img,height_, width_, channels);
                        
                        rgb_alpha_pixel p;
                        
                        if (!bgr){
                            p.red = static_cast<unsigned char>(addr[0]/255.0);
                            p.green = static_cast<unsigned char>(addr[1]/255.0);
                            p.blue = static_cast<unsigned char>(addr[2]/255.0);
                            p.alpha = static_cast<unsigned char>(addr[3]/255.0);
                        } else {
                            p.blue = static_cast<unsigned char>(addr[0]/255.0);
                            p.green = static_cast<unsigned char>(addr[1]/255.0);
                            p.red = static_cast<unsigned char>(addr[2]/255.0);
                            p.alpha = static_cast<unsigned char>(addr[3]/255.0);
                        }
                        
                        assign_pixel( t[n][m], p );
                        
                    }
                }
            }
            

}



// ----------------------------------------------------------------------------------------





// example usage

//see also http://dlib.net/imaging.html

    /*
    RGB
    There are two RGB pixel types in dlib, rgb_pixel and bgr_pixel. Each defines a 24bit RGB pixel type. The bgr_pixel is identical to rgb_pixel except that it lays the color channels down in memory in BGR order rather than RGB order and is therefore useful for interfacing with other image processing tools which expect this format (e.g. OpenCV). 
    */

matrix<rgb_pixel> return_matrix_of_rgb_pixel(int* original_img, unsigned height_, unsigned width_, unsigned bit_depth_, unsigned channels, bool bgr = 0){
    
    matrix<rgb_pixel> img;
    
    load_png_to_dlib_img(original_img, height_, width_, bit_depth_, channels, img, bgr);
    
    return img;
}

    /*
    RGB Alpha

    The rgb_alpha_pixel is an 8bit per channel RGB pixel with an 8bit alpha channel.
    */


matrix<rgb_alpha_pixel> return_matrix_of_rgb_alpha_pixel(int* original_img, unsigned height_, unsigned width_, unsigned bit_depth_, unsigned channels, bool bgr = 0){
    
    matrix<rgb_alpha_pixel> img;
    
    load_png_to_dlib_img(original_img, height_, width_, bit_depth_, channels, img, bgr);
    
    return img;
}

    /*
    Grayscale

    Any built in scalar type may be used as a grayscale pixel type. For example, unsigned char, int, double, etc.
    */

matrix<dlib::uint16> return_matrix_of_gray16_pixel(int* original_img, unsigned height_, unsigned width_, unsigned bit_depth_, unsigned channels, bool bgr = 0){
    
    matrix<dlib::uint16> img;
    
    load_png_to_dlib_img(original_img, height_, width_, bit_depth_, channels, img, bgr);
    
    return img;
}




//