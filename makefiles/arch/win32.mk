# ifeq "$(INSTALLDIR)" ""
#   INSTALLDIR=/usr/local
# endif
# IMAGEDIR = $(INSTALLDIR)/share/%APPLICATION_NAME%/icons
# 
CC = i586-mingw32msvc-gcc
AR = i586-mingw32msvc-ar rsu
LD = i586-mingw32msvc-g++

CFLAGS = -pipe -fno-exceptions -Wall -Wno-ctor-dtor-privacy -W -I /home/geometer/win32/zlib/include
# LDFLAGS = -Wl,-rpath,$(LIBDIR)
# 
# MOC = moc-qt3
# QTINCLUDE = -I /usr/include/qt3
# 
# GTKINCLUDE = $(shell pkg-config --cflags gtk+-2.0 libpng xft)
# 
# ifeq "$(UI_TYPE)" "qt"
# 	UILIBS = -lqt-mt
# else
# 	UILIBS = $(shell pkg-config --libs gtk+-2.0) -lpng -ljpeg
# endif
# 
# EXPATLIBS = -lexpat
# ENCALIBS = -lenca
# BZIP2LIBS = -lbz2
# 
# RM = rm -rvf
# RM_QUIET = rm -rf