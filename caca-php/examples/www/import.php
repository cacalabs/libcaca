<?php
header('Content-Type: text/html; charset=UTF-8');
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<?php
/*
 *  import        libcaca importers test program
 *  Copyright (c) 2008 Benjamin C. Wiley Sittler <bsittler@gmail.com>
 *
 *  This file is a Php port of "examples/import.c"
 *  which is: 
 *  Copyright (c) 2006 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

$imports = caca_get_import_list();

$file = isset($_FILES['file']) ? $_FILES['file']['tmp_name'] : NULL;
$filename = isset($_FILES['file']) ? $_FILES['file']['name'] : NULL;
$format = isset($_REQUEST['format']) ? $_REQUEST['format'] : NULL;

?>
<head>
<title><?= ($filename == NULL) ? '' : htmlspecialchars($filename . ' | ') ?>libcaca importers test program</title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
</head>
<body>
<?php

if ($file == NULL)
{
    ?>
<form id="importform" name="importform" action="#" enctype="multipart/form-data" method="post">
<label for="file">File:</label>
<input id="file" name="file" type="file" />
<br />
<input type="submit" value="Import" />
<label for="format">as</label>
<select name="format" id="format" onchange="update_preview(this);">
<?php
	foreach($imports as $import_format => $name)
	{
		?><option value="<?= htmlspecialchars($import_format) ?>"<?=
			($format == $import_format) ? ' selected="selected"' : '' ?>><?=
			htmlspecialchars($name . " (" . $import_format . ")") ?></option><?php
	}
?>
</select>
</form>
<?php
      ;
}

if($file)
{
	$cv = caca_create_canvas(0, 0);
	if(! $cv)
	{
		die("Can't create canvas\n");
	}
	
	if(caca_import_file($cv, $file, ($format == NULL) ? "" : $format) < 0)
	{
		die("could not import `" . htmlspecialchars($filename) . "'.\n");
	}
	
	echo caca_export_string($cv, "html3");
}

?>
</body>
</html>
