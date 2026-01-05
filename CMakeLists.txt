cmake_minimum_required(VERSION 3.21)
project(QtYoloDemo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------------------------
# Qt 配置 (ARM Qt 路径)
# --------------------------
set(CMAKE_PREFIX_PATH "/opt/Qt6/6.6.2/gcc_64/lib/cmake")  # 改成 ARM Qt 安装路径
find_package(Qt6 COMPONENTS Core Gui Widgets Multimedia REQUIRED)

# --------------------------
# OpenCV (ARM)
# --------------------------
find_package(OpenCV REQUIRED)

# --------------------------
# ONNX Runtime (ARM)
# --------------------------
set(ONNXRUNTIME_DIR "/opt/onnxruntime-arm")
find_package(onnxruntime REQUIRED PATHS ${ONNXRUNTIME_DIR}/lib/cmake/onnxruntime)

# --------------------------
# 源文件
# --------------------------
set(SOURCES
    main.cpp
    MainWindow.cpp MainWindow.h
    CameraWorker.cpp CameraWorker.h
    yolov8_onnx.cpp yolov8_onnx.h
)

# --------------------------
# 可执行文件
# --------------------------
add_executable(${PROJECT_NAME} ${SOURCES})

# --------------------------
# 链接库
# --------------------------
target_link_libraries(${PROJECT_NAME}
    Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Multimedia
    ${OpenCV_LIBS}
    onnxruntime::onnxruntime
)

# --------------------------
# 可选：ARM GPU / CUDA
# --------------------------
if(USE_CUDA)
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_CUDA)
endif()
