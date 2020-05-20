# 编译选项
set(SLIB libimage_segment)
set(SLIB_SRC 
graph_segmentation.cpp
graph_segmentation.h
image_graph.h)

add_library(${SLIB} SHARED ${SLIB_SRC})
# 依赖库
target_link_libraries(${SLIB} ${OpenCV_LIBS})