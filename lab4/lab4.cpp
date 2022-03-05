#include <iostream> // for std::cerr
#include <opencv2/imgproc/imgproc.hpp>  // for cv::cvtColor
#include <opencv2/highgui/highgui.hpp> // for cv::VideoCapture
#include <fstream> // for std::ofstream
#include <thread>
#include <vector>
#include <string>

// this is C :/
#include <unistd.h>
#include <stdint.h> // for uint32_t
#include <sys/ioctl.h> // for ioctl
#include <linux/fb.h> // for fb_
#include <fcntl.h> // for O_RDWR
#include <sys/time.h>


struct framebuffer_info { 
    uint32_t bits_per_pixel; uint32_t xres_virtual; 
};
struct framebuffer_info get_framebuffer_info(const char* framebuffer_device_path) {
    struct framebuffer_info info;
    struct fb_var_screeninfo screen_info;
    int fd = -1;
    fd = open(framebuffer_device_path, O_RDWR);
    if (fd >= 0) {
        if (!ioctl(fd, FBIOGET_VSCREENINFO, &screen_info)) {
            info.xres_virtual = screen_info.xres_virtual;
            info.bits_per_pixel = screen_info.bits_per_pixel;
        }
    }
    return info;
};

int main() {

    // recording
    const int frame_width = 650; // not sure
    const int frame_height = 600; // not sure
    const int frame_rate = 10;
    framebuffer_info fb_info = get_framebuffer_info("/dev/fb0");
    std::ofstream ofs("/dev/fb0");
    cv::Mat img = cv::imread("/root/sample.bmp", cv::IMREAD_COLOR);
    cv::Mat img_r;
    cv::resize(img, img_r, cv::Size(frame_width, frame_height), cv::INTER_LINEAR);
    std::vector<cv::Mat> vimgs;
    vimgs.push_back(img_r);
    vimgs.push_back(img_r);
    vimgs.push_back(img_r);
    cv::Mat frame;
    cv::vconcat(vimgs,frame);
    //
    int cnt = 0;
    int rate = 5;
    while(1){
        if (frame.depth() != CV_8U) {
            std::cerr << "Not 8 bits per pixel and channel." << std::endl;
        } else if (frame.channels() != 3) {
            std::cerr << "Not 3 channels." << std::endl;
        } else {
            // 3 Channels (assumed BGR), 8 Bit per Pixel and Channel
            int framebuffer_width = fb_info.xres_virtual;
            int framebuffer_depth = fb_info.bits_per_pixel;
            cv::Size2f frame_size = img_r.size();
            cv::Mat framebuffer_compat;
            cv::Mat framebuffer_max;
            cv::cvtColor(frame, framebuffer_compat, cv::COLOR_BGR2BGR565);
            for (int y = 0; y < frame_size.height ; y++) {
                ofs.seekp(y*framebuffer_width*2);
                ofs.write(reinterpret_cast<char*>(framebuffer_compat.ptr((y+cnt*rate)%(int)(frame_size.height*3))),frame_size.width*2);
            }
        }
        cnt++;
        //printf("%d %d\n",i,cnt);
    }
}