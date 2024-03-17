find_path(DPP_INCLUDE_DIR NAMES dpp/dpp.h HINTS "${DPP_ROOT_DIR}/include")
	 
find_library(DPP_LIBRARIES NAMES libdpp.10.0.30.dylib HINTS "${DPP_ROOT_DIR}/lib")
	 
include(FindPackageHandleStandardArgs)
	 
find_package_handle_standard_args(DPP DEFAULT_MSG DPP_LIBRARIES DPP_INCLUDE_DIR)
