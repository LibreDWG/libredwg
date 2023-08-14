## Makefile.am
##
## Copyright (C) 2023 Free Software Foundation, Inc.
##
## This file is part of LibreDWG.

ALL_CODEPAGES = UTF8 US-ASCII ISO-8859-1 ISO-8859-2 ISO-8859-3 ISO-8859-4 ISO-8859-5 \
	ISO-8859-6 ISO-8859-7 ISO-8859-8 ISO-8859-9 CP437 CP850 CP852 CP855 \
	CP857 CP860 CP861 CP863 CP864 CP865 CP869 CP932 MACINTOSH BIG5 CP949 \
	JOHAB CP866 WINDOWS-1250 WINDOWS-1251 WINDOWS-1252 GB2312 WINDOWS-1253 \
	WINDOWS-1254 WINDOWS-1255 WINDOWS-1256 WINDOWS-1257 WINDOWS-874 \
	WINDOWS-932 WINDOWS-936 WINDOWS-949 WINDOWS-950 WINDOWS-1361 UTF16 \
	WINDOWS-1258

ISO8859_URL = https://www.unicode.org/Public/MAPPINGS/ISO8859
ISO8859_TXT = 8859-2.TXT 8859-3.TXT 8859-4.TXT 8859-5.TXT 8859-6.TXT \
	8859-7.TXT 8859-8.TXT 8859-9.TXT
PC_URL = https://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/PC
PC_TXT = CP437.TXT CP850.TXT CP852.TXT CP855.TXT CP857.TXT CP860.TXT \
	CP861.TXT CP863.TXT CP864.TXT CP865.TXT CP866.TXT CP869.TXT
MS_URL = https://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WINDOWS
MS_TXT = CP874.TXT CP932.TXT CP936.TXT CP949.TXT CP950.TXT CP1250.TXT \
	CP1251.TXT CP1252.TXT CP1253.TXT CP1254.TXT CP1255.TXT CP1256.TXT \
	CP1257.TXT CP1258.TXT
EAST1_URL = https://www.unicode.org/Public/MAPPINGS/OBSOLETE/EASTASIA/JIS
EAST1_TXT = SHIFTJIS.TXT
EAST2_URL = https://www.unicode.org/Public/MAPPINGS/OBSOLETE/EASTASIA/KSC
EAST2_TXT = JOHAB.TXT
EAST3_URL = https://www.unicode.org/Public/MAPPINGS/OBSOLETE/EASTASIA/OTHER
EAST3_TXT = BIG5.TXT
GB_URL    = https://raw.githubusercontent.com/freebsd/freebsd-src/main/tools/tools/locale/etc/charmaps
GB_TXT    = GB2312.TXT
ALL_TXT = $(ISO8859_TXT) $(PC_TXT) $(MS_TXT) MACINTOSH.TXT $(EAST1_TXT) $(EAST2_TXT) \
	$(EAST3_TXT) $(GB_TXT) 

EXTRA_HEADERS = ISO-8859-2.h ISO-8859-3.h ISO-8859-4.h ISO-8859-5.h \
	ISO-8859-6.h ISO-8859-7.h ISO-8859-8.h ISO-8859-9.h CP437.h CP850.h \
	CP852.h CP855.h CP857.h CP860.h CP861.h CP863.h CP864.h CP865.h CP869.h \
	CP932.h MACINTOSH.h BIG5.h CP949.h JOHAB.h CP866.h WINDOWS-1250.h \
	WINDOWS-1251.h WINDOWS-1252.h GB2312.h WINDOWS-1253.h \
	WINDOWS-1254.h WINDOWS-1255.h WINDOWS-1256.h WINDOWS-1257.h WINDOWS-874.h \
	WINDOWS-932.h WINDOWS-936.h WINDOWS-949.h WINDOWS-950.h WINDOWS-1361.h \
	WINDOWS-1258.h
EXTRA_DIST = $(EXTRA_HEADERS) txt2h.pl

all: $(EXTRA_HEADERS)

.PHONY: get-sources
get-sources:
	for c in $(ISO8859_TXT); do wget -qN $(ISO8859_URL)/$$c -O $(srcdir)/ISO-$$c; done
	wget -qN https://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/MAC/ROMAN.TXT -O $(srcdir)/MACINTOSH.TXT
	wget -qN https://opensource.apple.com/source/cups/cups-136.18/cups/data/windows-1361.txt  -O $(srcdir)/WINDOWS-1361.TXT
	cd $(srcdir) && \
	  for c in $(ISO8859_TXT); do wget -qN $(ISO8859_URL)/$$c; done; \
	  for c in $(PC_TXT); do wget -qN $(PC_URL)/$$c; done; \
	  for c in $(MS_TXT); do d=$$(echo $$c|sed 's,^CP,WINDOWS-,'); \
	    wget -qN $(MS_URL)/$$c -O $$d; done; \
	  for c in $(EAST1_TXT); do wget -qN $(EAST1_URL)/$$c; done; \
	  mv $(EAST1_TXT) CP932.TXT; \
	  for c in $(EAST2_TXT); do wget -qN $(EAST2_URL)/$$c; done; \
	  for c in $(EAST3_TXT); do wget -qN $(EAST3_URL)/$$c; done; \
	  for c in $(GB_TXT); do wget -qN $(GB_URL)/$$c; done; \
	cd $(abs_builddir)

.SUFFIXES = .TXT .h
.TXT.h:
	$(srcdir)/txt2h.pl $< >$@ && mv -fu $@ $(srcdir)/
