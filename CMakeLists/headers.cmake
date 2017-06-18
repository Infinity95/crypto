###########
# Headers #
###########

set(MyTestHeaders

)
assign_source_group(${MyTestHeaders})

set(MyTestInternalHeaders
    "${CMAKE_SOURCE_DIR}/src/algorithms/SymmetricBlockCipher.hpp"
    "${CMAKE_SOURCE_DIR}/src/algorithms/FEAL.hpp"
    # Utility headers
    "${CMAKE_SOURCE_DIR}/src/util/Logging.hpp"
    "${CMAKE_SOURCE_DIR}/src/util/Strings.hpp"
    "${CMAKE_SOURCE_DIR}/src/util/TickCounter.hpp"
    "${CMAKE_SOURCE_DIR}/src/util/TickCounter.inl"
)
assign_source_group(${MyTestInternalHeaders})