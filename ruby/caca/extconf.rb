require 'mkmf'

pkg_config("caca")
$CFLAGS += " -Wall"
create_makefile("caca")
