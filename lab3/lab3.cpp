#include <iostream> // for std::cerr
#include <opencv2/imgproc/imgproc.hpp>  // for cv::cvtColor
#include <opencv2/highgui/highgui.hpp> // for cv::VideoCapture
#include <fstream> // for std::ofstream
#include <thread>
#include <string>

// this is C :/
#include <unistd.h>
#include <stdint.h> // for uint32_t
#include <sys/ioctl.h> // for ioctl
#include <linux/fb.h> // for fb_
#include <fcntl.h> // for O_RDWR

/************************** Bonus ******************************/
bool key_flg = false;
bool end_flg = false;
int cnt = 0;
void snapshot(cv::Mat frame){
    std::string path = "/run/media/mmcblk1p1/"+std::to_string(cnt)+".png";
    cv::imwrite(path,frame);
    cnt++;
    //std::cout << "Successfully snapshot" << std::endl;
    printf("Successfully snapshot\n");
}
void wait_input(){
    //std::cout << "Start waiting input" << std::endl;
    printf("Start waiting input\n");
    cv::namedWindow("Display Window",cv::WINDOW_AUTOSIZE);
    while(1){
        int input = cv::waitKey(0);
        printf("input=%d\n",input);
        if(input == 99){
            key_flg = true;
        }
        else if(input == 100){
            end_flg = true;
            break;
        }
    }
    cv::destroyAllWindows();
    return;
}
/***************************************************************/

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

    // create thread for waiting key
    std::thread t1(wait_input);

    // recording
    const int frame_width = 850; // not sure
    const int frame_height = 600; // not sure
    const int frame_rate = 10;
    framebuffer_info fb_info = get_framebuffer_info("/dev/fb0");
    cv::VideoCapture cap(2);
    if(!cap.isOpened()) {
        std::cerr << "Could not open video device." << std::endl;
        return 1;
    } else {
        std::cout << "Successfully opened video device." << std::endl;
        cap.set(CV_CAP_PROP_FRAME_WIDTH,frame_width);
        cap.set(CV_CAP_PROP_FRAME_HEIGHT,frame_height);
        cap.set(CV_CAP_PROP_FPS,frame_rate);
        std::ofstream ofs("/dev/fb0");
        cv::Mat frame;
        // Bonus 2
        cv::Size S = cv::Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH),(int)cap.get(CV_CAP_PROP_FRAME_HEIGHT));
        cv::VideoWriter writer("/run/media/mmcblk1p1/bonus.avi",CV_FOURCC('M','J','P','G'),frame_rate,S,true);
        //
        while (true) {
            cap >> frame;
            /******** Bonus  ****************/
            writer.write(frame);
            if(key_flg==true){
                snapshot(frame);
                key_flg = false;
            }
            if(end_flg==true){
                printf("Recording ends\n");
                cap.release();
                writer.release();
                t1.join();
                return 0;
            }
            /******************************/
            if (frame.depth() != CV_8U) {
                std::cerr << "Not 8 bits per pixel and channel." << std::endl;
            } else if (frame.channels() != 3) {
                std::cerr << "Not 3 channels." << std::endl;
            } else {
                // 3 Channels (assumed BGR), 8 Bit per Pixel and Channel
                int framebuffer_width = fb_info.xres_virtual;
                int framebuffer_depth = fb_info.bits_per_pixel;
                cv::Size2f frame_size = frame.size();
                cv::Mat framebuffer_compat;
                cv::Mat framebuffer_max;
                switch (framebuffer_depth) {
                    case 16:
                        cv::cvtColor(frame, framebuffer_compat, cv::COLOR_BGR2BGR565);
                        for (int y = 0; y < frame_size.height ; y++) {
                            ofs.seekp(y*framebuffer_width*2);
                            ofs.write(reinterpret_cast<char*>(framebuffer_compat.ptr(y)),frame_size.width*2);
                        }
                    break;
                    case 32: {
                            std::vector<cv::Mat> split_bgr;
                            cv::split(frame, split_bgr);
                            split_bgr.push_back(cv::Mat(frame_size,CV_8UC1,cv::Scalar(255)));
                            cv::merge(split_bgr, framebuffer_compat);
                            for (int y = 0; y < frame_size.height ; y++) {
                                ofs.seekp(y*framebuffer_width*4);
                                ofs.write(reinterpret_cast<char*>(framebuffer_compat.ptr(y)),frame_size.width*4);
                            }
                        } break;
                    default:
                        std::cerr << "Unsupported depth of framebuffer." << std::endl;
                }
            }
        }
    }
}