###########
# Sources #
###########

set(MyTestSrc
	"${CMAKE_SOURCE_DIR}/src/algorithms/FEALLinearCA.cpp"
    "${CMAKE_SOURCE_DIR}/src/algorithms/FEAL.cpp"
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
)
assign_source_group(${MyTestSrc})