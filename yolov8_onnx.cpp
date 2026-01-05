#include "yolov8_onnx.h"
#include <onnxruntime_cxx_api.h>
#include <algorithm>

YOLOv8::YOLOv8(const std::string &onnx_path, bool use_cuda)
    : env_(ORT_LOGGING_LEVEL_WARNING, "yolo")
{
    session_options_.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
    if(use_cuda){
        #ifdef USE_CUDA
        Ort::ThrowOnError(OrtSessionOptionsAppendExecutionProvider_CUDA(session_options_, 0));
        #endif
    }
    session_ = Ort::Session(env_, onnx_path.c_str(), session_options_);

    // 获取输入输出名字
    Ort::AllocatorWithDefaultOptions allocator;
    input_names_.push_back(session_.GetInputName(0, allocator));
    output_names_.push_back(session_.GetOutputName(0, allocator));
}

cv::Mat YOLOv8::preprocess(const cv::Mat &image) {
    cv::Mat resized;
    cv::resize(image, resized, cv::Size(input_width_, input_height_));
    cv::cvtColor(resized, resized, cv::COLOR_BGR2RGB);
    resized.convertTo(resized, CV_32F, 1/255.0);
    return resized;
}

std::vector<Detection> YOLOv8::postprocess(const cv::Mat &image, const std::vector<float> &output){
    std::vector<Detection> results;
    int num_boxes = output.size() / 85; // YOLOv8n 85 = 4+1+80
    float x_factor = image.cols / 640.0;
    float y_factor = image.rows / 640.0;

    for(int i=0;i<num_boxes;i++){
        float conf = output[i*85+4];
        if(conf < conf_threshold_) continue;

        int class_id = std::max_element(output.begin()+i*85+5, output.begin()+i*85+85) - (output.begin()+i*85+5);
        float score = conf * output[i*85+5+class_id];
        if(score < conf_threshold_) continue;

        float cx = output[i*85];
        float cy = output[i*85+1];
        float w = output[i*85+2];
        float h = output[i*85+3];

        int x = (cx - w/2)*x_factor;
        int y = (cy - h/2)*y_factor;
        int width = w*x_factor;
        int height = h*y_factor;

        results.push_back({cv::Rect(x,y,width,height), class_id, score});
    }
    return results;
}

std::vector<Detection> YOLOv8::detect(const cv::Mat &image) {
    cv::Mat input = preprocess(image);
    std::vector<float> input_data(input.total()*input.channels());
    std::memcpy(input_data.data(), input.data, input_data.size()*sizeof(float));

    std::array<int64_t,4> input_shape{1,3,input_height_,input_width_};
    Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

    Ort::Value input_tensor = Ort::Value::CreateTensor<float>(memory_info,
        input_data.data(),
        input_data.size(),
        input_shape.data(),
        input_shape.size());

    auto output_tensors = session_.Run(Ort::RunOptions{nullptr},
        input_names_.data(), &input_tensor, 1,
        output_names_.data(), 1);

    float* out_data = output_tensors[0].GetTensorMutableData<float>();
    size_t out_size = output_tensors[0].GetTensorTypeAndShapeInfo().GetElementCount();
    std::vector<float> output_vec(out_data, out_data+out_size);

    return postprocess(image, output_vec);
}
