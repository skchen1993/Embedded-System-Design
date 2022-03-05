#include <fcntl.h> 
#include <fstream>
#include <iostream>
#include <linux/fb.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <sys/ioctl.h>

//read image 
#include <opencv2/imgcodecs.hpp>

struct framebuffer_info
{
    uint32_t bits_per_pixel;    // framebuffer depth
    uint32_t xres_virtual;      // how many pixel in a row in virtual screen
};

struct framebuffer_info get_framebuffer_info(const char *framebuffer_device_path);

int main(int argc, const char *argv[])
{
    cv::Mat image;
    cv::Size2f image_size;
    framebuffer_info fb_info = get_framebuffer_info("/dev/fb0");
    std::ofstream ofs("/dev/fb0");

    // read image file (sample.bmp) from opencv libs.
    image = cv::imread("sample.bmp", cv::IMREAD_COLOR);

    // get image size of the image.
    image_size = image.size();

    // transfer color space from BGR to BGR565 (16-bit image) to fit the requirement of the LCD
    cv::Mat image_BGR565;
    cv::cvtColor(image, image_BGR565, cv::COLOR_BGR2BGR565);

    // output to framebufer row by row
    int framebuffer_width = fb_info.xres_virtual;
    int framebuffer_depth = fb_info.bits_per_pixel;
    const int BYTES_PER_PIXEL = fb_info.bits_per_pixel / 8;
    for (int y = 0; y < image_size.height; y++)
    {
        // move to the next written position of output device framebuffer by "std::ostream::seekp()".
        // posisiotn can be calcluated by "y", "fb_info.xres_virtual", and "fb_info.bits_per_pixel".
        ofs.seekp(y*framebuffer_width*BYTES_PER_PIXEL);

        // write to the framebuffer by "std::ostream::write()".
        // you could use "cv::Mat::ptr()" to get the pointer of the corresponding row.
        // you also have to count how many bytes to write to the buffer
        ofs.write(reinterpret_cast<char*>(image_BGR565.ptr(y)),image_size.width*BYTES_PER_PIXEL);
    }
    return 0;
}

struct framebuffer_info get_framebuffer_info(const char *framebuffer_device_path)
{
    struct framebuffer_info fb_info;        // Used to return the required attrs.
    struct fb_var_screeninfo screen_info;   // Used to get attributes of the device from OS kernel.

    // open device with linux system call "open()"
    int fd = open(framebuffer_device_path,O_RDWR);
    if(!fd)
        std::cerr << "open frame buffer device error" << std::endl;

    // get attributes of the framebuffer device thorugh linux system call "ioctl()".
    // the command you would need is "FBIOGET_VSCREENINFO"
    int val = ioctl(fd,FBIOGET_VSCREENINFO,&screen_info);
    if(val)
        std::cerr << "ioctl error" << std::endl;

    // put the required attributes in variable "fb_info" you found with "ioctl() and return it."
    fb_info.xres_virtual = screen_info.xres_virtual;      // 8
    fb_info.bits_per_pixel = screen_info.bits_per_pixel;    // 16

    return fb_info;
};