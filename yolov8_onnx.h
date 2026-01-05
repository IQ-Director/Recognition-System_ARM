#pragma once
#include <opencv2/opencv.hpp>
#include <onnxruntime_cxx_api.h>
#include <vector>
#include <string>

struct Detection {
    cv::Rect box;
    int class_id;
    float score;
};

class YOLOv8 {
public:
    YOLOv8(const std::string &onnx_path, bool use_cuda = true);
    std::vector<Detection> detect(const cv::Mat &image);

private:
    Ort::Env env_;
    Ort::SessionOptions session_options_;
    Ort::Session session_{nullptr};
    std::vector<const char*> input_names_;
    std::vector<const char*> output_names_;
    int input_width_ = 640;
    int input_height_ = 640;
    float conf_threshold_ = 0.25;
    float iou_threshold_ = 0.45;

    cv::Mat preprocess(const cv::Mat &image);
    std::vector<Detection> postprocess(const cv::Mat &image, const std::vector<float> &output);
};
