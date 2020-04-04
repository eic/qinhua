AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = \
    libqinhuahistos.la

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib

libqinhuahistos_la_LDFLAGS = \
   -L$(libdir) \
   -L$(OFFLINE_MAIN)/lib \
   -lphg4hit \
   -lg4detectors

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include \
  -I$(G4_MAIN)/include

pkginclude_HEADERS = \
  CellNtuple.h \
  HitCombiner.h \
  HitNtuple.h

libqinhuahistos_la_SOURCES = \
  CellNtuple.cc \
  HitCombiner.cc \
  HitNtuple.cc

################################################
# linking tests

noinst_PROGRAMS = \
  testexternals

BUILT_SOURCES = testexternals.cc

testexternals_SOURCES = testexternals.cc
testexternals_LDADD = libqinhuahistos.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@


clean-local:
	rm -f *Dict* $(BUILT_SOURCES) *.pcm
