PHP_ARG_ENABLE(caca, whether to enable Libcaca support,
[ --enable-caca   Enable Libcaca support])

if test "$PHP_CACA" = "yes"; then
	AC_CHECK_LIB(caca, caca_get_version, [
		PHP_ADD_LIBRARY(caca,, CACA_SHARED_LIBADD)
	], [
		AC_MSG_ERROR(libcaca required !)
	])
	PHP_NEW_EXTENSION(caca, php_caca.c, $ext_shared,,)
	PHP_SUBST(CACA_SHARED_LIBADD)
fi 
