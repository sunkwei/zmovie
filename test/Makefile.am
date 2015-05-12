AUTOMAKE_OPTIONS = foreign

CFLAGS= -Wall -O2 -g

bin_PROGRAMS= test_ap


test_ap_SOURCES=	webrtc_apm.cpp
test_ap_LDADD=	$(WEBRTC_AP_LIBS) $(SNDFILE_LIBS) -lpthread
