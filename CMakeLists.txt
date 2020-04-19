AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/RecruitCommand.cpp")
AC_ADD_SCRIPT_LOADER("RecruitCommand" "${CMAKE_CURRENT_LIST_DIR}/src/loader.h")

AC_ADD_CONFIG_FILE("${CMAKE_CURRENT_LIST_DIR}/conf/mod_recruit_friend.conf.dist")
