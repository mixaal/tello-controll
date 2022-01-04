AC_INIT
AC_CONFIG_SRCDIR([main.c])
AC_CONFIG_HEADERS([config.h])
AC_PROG_CC
AC_PROG_CXX
AC_CHECK_HEADERS([threads.h])
AC_CHECK_HEADERS([pthread.h])
AC_CHECK_HEADER_STDBOOL

AC_CHECK_FUNCS([floor]) 
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([socket])
AC_CHECK_FUNCS([strdup])
AC_CHECK_HEADERS([arpa/inet.h]) 
AC_CHECK_HEADERS([fcntl.h]) 
AC_CHECK_HEADERS([netinet/in.h])
AC_CHECK_HEADERS([stddef.h])
AC_CHECK_HEADERS([sys/ioctl.h]) 
AC_CHECK_HEADERS([sys/socket.h])
AC_FUNC_MALLOC 
AC_TYPE_INT32_T
AC_TYPE_SIZE_T 
AC_TYPE_SSIZE_T

AC_CHECK_LIB(mnl,  main, [], [
   echo "Error - you need wireless lib: sudo apt install libmnl-dev"
   exit -1
])


AC_CHECK_LIB(m,  main, [], [
   echo "Error - you need math lib!"
   exit -1
])

AC_CHECK_LIB(avcodec,  main, [], [
   echo "Error - you need avcodec lib!"
   exit -1
])
#AC_CHECK_LIB(avformat,  main, [], [
#   echo "Error - you need avformat lib!"
#   exit -1
#])
AC_CHECK_LIB(avutil,  main, [], [
   echo "Error - you need avutil lib!"
   exit -1
])
AC_CHECK_LIB(pthread,  main, [], [
   echo "Error - you need pthread lib!"
   exit -1
])
AC_CHECK_LIB(SDL2,  main, [], [
   echo "Error - you need sdl2 lib!"
   exit -1
])
AC_CHECK_LIB(SDL2_image,  main, [], [
   echo "Error - you need sdl2 image lib!"
   exit -1
])

AC_CHECK_LIB(SDL2_mixer,  main, [], [
   echo "Error - you need sdl2 mixer lib!"
   exit -1
])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

