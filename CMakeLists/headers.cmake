###########
# Headers #
###########

set(MyTestHeaders

)
assign_source_group(${MyTestHeaders})

set(MyTestInternalHeaders
    # Utility headers
    "${CMAKE_SOURCE_DIR}/src/util/Logging.hpp"
    "${CMAKE_SOURCE_DIR}/src/util/Strings.hpp"
    "${CMAKE_SOURCE_DIR}/src/util/TickCounter.hpp"
    "${CMAKE_SOURCE_DIR}/src/util/TickCounter.inl"
)
assign_source_group(${MyTestInternalHeaders})