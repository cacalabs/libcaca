#!/usr/bin/php5
<?

echo "libcaca version: ".caca_get_version()."\n";
echo "Available drivers:\n";
$list = caca_get_display_driver_list();
foreach($list as $type => $name)
	echo "* $name ($type)\n";
