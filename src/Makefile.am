bin_PROGRAMS = \
    agesture

AM_CFLAGS = \
    $(CLUTTER_CFLAGS) \
    $(GSTREAMER_CFLAGS) \
    $(LIBLO_CFLAGS) \
    -I$(top_srcdir)

LDADD = \
    $(CLUTTER_LIBS) \
    $(GSTREAMER_LIBS) \
    $(LIBLO_LIBS)

agesture_CXXFLAGS = $(AM_CFLAGS)
agesture_LDADD = $(LDADD)

EXTRA_DIST = \
    nearestneighborfinder.h

agesture_SOURCES = \
    main.cpp \
    nearestneighborfinder.cpp

