#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
using namespace cv;
using namespace std;
using namespace face;


// 创建和返回一个归一化后的图像矩阵:
static Mat norm_0_255(InputArray _src) {    
    Mat src = _src.getMat();          
    Mat dst;    
    switch (src.channels()) {    
        case1:        
            cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC1);        
            break;    
        case3:       
            cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC3);        
            break;    
        default:        
            src.copyTo(dst);        
            break;    
    }   
    return dst; 
}
//使用CSV文件去读图像和标签，主要使用stringstream和getline方法
static void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';') {    
    std::ifstream file(filename.c_str(), ifstream::in);    
    if (!file) {        
        string error_message = "No valid input file was given, please check the given filename.";        
        //CV_Error(CV_StsBadArg, error_message);   
        cout << error_message << endl;
    }    
    string line, path, classlabel;    
    while (getline(file, line)) {        
        stringstream liness(line);        
        getline(liness, path, separator);        
        getline(liness, classlabel);        
        if (!path.empty() && !classlabel.empty()) {            
        images.push_back(imread(path, 0));            
        labels.push_back(atoi(classlabel.c_str()));       
        }  
    }
}
int main(){    
    //读取你的CSV文件路径.    
    //string fn_csv = string(argv[1]);    
    string fn_csv = "./at.txt";    
    // 2个容器来存放图像数据和对应的标签    
    vector<Mat> images;    
    vector<int> labels;    
    
    // 读取数据. 如果文件不合法就会出错    
    // 输入的文件名已经有了.   
    try   
    {        
        read_csv(fn_csv, images, labels);    
    }   
    catch (cv::Exception& e)    
    {        
        cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << endl;        
        // 文件有问题，我们啥也做不了了，退出了        
        exit(1);    
    }    
    
    // 如果没有读取到足够图片，也退出.   
    if (images.size() <= 1) {        
        string error_message = "This demo needs at least 2 images to work. Please add more images to your data set!";        
        //CV_Error(CV_StsError, error_message);   
        cout << error_message << endl; 
    }    
    
    // 下面的几行代码仅仅是从你的数据集中移除最后一张图片    
    //[gm:自然这里需要根据自己的需要修改，他这里简化了很多问题] 
    const int TEST_SIZE = 1;
    vector<Mat> testSamples;
    vector<int> testLabels;
    for(int i = 1;i <= TEST_SIZE;i++){
        testSamples.push_back(images[images.size()-1]);
        testLabels.push_back(labels[labels.size()-1]);
        images.pop_back();
        labels.pop_back();
    }     
    // 
    Ptr<FaceRecognizer> model = EigenFaceRecognizer::create();    
    model->train(images, labels);    
    model->save("MyFacePCAModel.xml");    
    Ptr<FaceRecognizer> model1 = FisherFaceRecognizer::create();    
    model1->train(images, labels);    
    model1->save("MyFaceFisherModel.xml");   
    Ptr<FaceRecognizer> model2 = LBPHFaceRecognizer::create();    
    model2->train(images, labels);    
    model2->save("MyFaceLBPHModel.xml");   
     // 下面对测试图像进行预测，predictedLabel是预测标签结果    
    int predictedLabel;
    double confidence;
    int correct = 0;
    int correct1 = 0;
    int correct2  = 0;
    for(int i = 0;i < TEST_SIZE;i++){
        model->predict(testSamples[i],predictedLabel,confidence);
        if(predictedLabel == testLabels[i]){
            correct++;
        }
        model1->predict(testSamples[i],predictedLabel,confidence);
        if(predictedLabel == testLabels[i]){
            correct1++;
        }
        model2->predict(testSamples[i],predictedLabel,confidence);
        if(predictedLabel == testLabels[i]){
            correct2++;
        }
    }
    cout << "Model PCA accuracy: " << (double)correct<< endl;
    cout << "Model Fisher accuracy: " << (double)correct1 << endl;
    cout << "Model LBPH accuracy: " << (double)correct2 << endl;
    waitKey(0);    
    return 0;
 }   