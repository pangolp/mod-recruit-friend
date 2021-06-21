
# Add cpp files
AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/RecruitCommand.cpp")

# Add scripts to script loader
AC_ADD_SCRIPT_LOADER("RecruitCommand" "${CMAKE_CURRENT_LIST_DIR}/src/loader.h")
