
jnidir = $(libdir)/jni
libcaca_javadir = $(datadir)/java

java_jar = libjava.jar

if USE_JAVA
jni_LTLIBRARIES = libcaca-java.la
libcaca_java_DATA = $(java_jar)
noinst_DATA = examples/Driver.class examples/TrueColor.class
endif

java_src = $(wildcard $(srcdir)/org/zoy/caca/*.java)

java_jni_headers = \
    org_zoy_caca_Attribute.h \
    org_zoy_caca_Caca.h \
    org_zoy_caca_Canvas.h \
    org_zoy_caca_Display.h \
    org_zoy_caca_Dither.h \
    org_zoy_caca_Event.h \
    org_zoy_caca_Font.h

java_jni_src = \
    org_zoy_caca_Attribute.c \
    org_zoy_caca_Caca.c \
    org_zoy_caca_Canvas.c \
    org_zoy_caca_Display.c \
    org_zoy_caca_Dither.c \
    org_zoy_caca_Event.c \
    org_zoy_caca_Font.c

java_jni_classes = \
    org/zoy/caca/Attribute.class \
    org/zoy/caca/Caca.class \
    org/zoy/caca/Canvas.class \
    org/zoy/caca/Display.class \
    org/zoy/caca/Dither.class \
    org/zoy/caca/Event.class \
    org/zoy/caca/Font.class

java_jni_java = \
    org/zoy/caca/CacaException.java \
    org/zoy/caca/Caca.java \
    org/zoy/caca/NativeObject.java \
    org/zoy/caca/Dither.java \
    org/zoy/caca/Canvas.java \
    org/zoy/caca/CacaObject.java \
    org/zoy/caca/Font.java \
    org/zoy/caca/Attribute.java \
    org/zoy/caca/Color.java \
    org/zoy/caca/Event.java \
    org/zoy/caca/TimeoutException.java \
    org/zoy/caca/Display.java

JAVAH_FLAGS = -d . -classpath .

$(java_jni_classes): $(java_src)
	$(JAVAC) -d . $(java_src)
	$(JAR) -cf $(java_jar) org

# Javah does not update the file if it detects that there is nothing to do,
# hence the `touch` so that javah is not run at every `make`.

org_zoy_caca_Attribute.h: org/zoy/caca/Attribute.class
	$(JAVAH) $(JAVAH_FLAGS) org.zoy.caca.Attribute
	@touch org_zoy_caca_Attribute.h

org_zoy_caca_Caca.h: org/zoy/caca/Caca.class
	$(JAVAH) $(JAVAH_FLAGS) org.zoy.caca.Caca
	@touch org_zoy_caca_Caca.h

org_zoy_caca_Canvas.h: org/zoy/caca/Canvas.class
	$(JAVAH) $(JAVAH_FLAGS) org.zoy.caca.Canvas
	@touch org_zoy_caca_Canvas.h

org_zoy_caca_Display.h: org/zoy/caca/Display.class
	$(JAVAH) $(JAVAH_FLAGS) org.zoy.caca.Display
	@touch org_zoy_caca_Display.h

org_zoy_caca_Dither.h: org/zoy/caca/Dither.class
	$(JAVAH) $(JAVAH_FLAGS) org.zoy.caca.Dither
	@touch org_zoy_caca_Dither.h

org_zoy_caca_Event.h: org/zoy/caca/Event.class
	$(JAVAH) $(JAVAH_FLAGS) org.zoy.caca.Event
	@touch org_zoy_caca_Event.h

org_zoy_caca_Font.h: org/zoy/caca/Font.class
	$(JAVAH) $(JAVAH_FLAGS) org.zoy.caca.Font
	@touch org_zoy_caca_Font.h

CLEANFILES = $(noinst_DATA) $(java_jar) \
             org/zoy/caca/*.class org_zoy_caca_[A-Z]*.h
EXTRA_DIST = README $(java_jni_java) \
             examples/Driver.java examples/TrueColor.java

if USE_JAVA
BUILT_SOURCES = $(java_jni_headers) $(java_jni_classes)
endif

libcaca_java_la_CPPFLAGS = $(JAVA_CFLAGS)
libcaca_java_la_SOURCES = \
  caca_java_common.h   \
  caca_java_common.c   \
  $(java_jni_src)
#nodist_libcaca_java_la_SOURCES = \
#  $(java_jni_headers)
libcaca_java_la_LDFLAGS = -shared $(JAVA_LIBS)
libcaca_java_la_LIBADD = ../caca/libcaca.la

examples/Driver.class: examples/Driver.java
	$(JAVAC) $^

examples/TrueColor.class: examples/TrueColor.java
	$(JAVAC) $^

