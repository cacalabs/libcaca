require 'mkmf'

pkg_config("cucul")
$CFLAGS += " -Wall"
create_makefile("cucul")
