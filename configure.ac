# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT(agesture, [0.1.1], [http://github.com/aalex/agesture/issues], [agesture], [http://github.com/aalex/agesture])
AC_CONFIG_MACRO_DIR([m4])
AC_MSG_NOTICE([Configuring agesture])
AM_INIT_AUTOMAKE([subdir-objects -Wno-portability])
AC_CONFIG_HEADERS([src/config.h])
AC_CANONICAL_HOST # fills $host variable

# Checks for programs.
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LIBTOOL

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for libraries
AC_CHECK_LIB([m],[sqrt])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h stddef.h stdlib.h string.h sys/time.h])

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_CHECK_FUNCS([gettimeofday memmove mkdir setenv sqrt strchr strpbrk uname])

AX_PTHREAD

CXXFLAGS+=" -Wall "
dnl -Wextra -Wfatal-errors -Werror -Wnon-virtual-dtor "

#Set LIBLO with pkg-config
PKG_CHECK_MODULES(LIBLO, liblo, have_liblo=true, have_liblo=false)
if test "x${have_liblo}" = "xfalse" ; then
    AC_MSG_ERROR([Missing liblo])
fi

PKG_CHECK_MODULES(CLUTTER, clutter-1.0, have_clutter=true, have_clutter=false)
if test "x${have_clutter}" = "xfalse" ; then
    AC_MSG_ERROR([Missing libclutter-1.0])
fi

PKG_CHECK_MODULES(GSTREAMER, glib-2.0 gstreamer-0.10, have_gstreamer=true, have_gstreamer=false)
if test "x${have_gstreamer}" = "xfalse" ; then
    AC_MSG_ERROR([Missing libgstreamer-0.10 or glib-2.0])
fi

# Override the template file name of the generated .pc file, so that there
# is no need to rename the template file when the API version changes.
AC_CONFIG_FILES([
        Makefile 
        src/Makefile 
        ])

AC_OUTPUT

