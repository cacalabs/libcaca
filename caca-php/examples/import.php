#!/usr/bin/php5
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
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

if (php_sapi_name() != "cli") {
    die("You have to run this program with php-cli!\n");
}

$imports = caca_get_import_list();

if($argc < 2 || $argc > 3)
{
    $msg = ($argv[0] . ": wrong argument count\n" .
            "usage: " . $argv[0] . " file [<format>]\n" .
            "where <format> is one of:\n");
    foreach($imports as $format => $name)
        $msg .= " \"" . $name . "\" (" . $format . ")\n";
    die($msg);
}

$cv = caca_create_canvas(0, 0);
if(! $cv)
{
    die("Can't create canvas\n");
}

if(caca_import_file($cv, $argv[1], $argc >= 3 ? $argv[2] : "") < 0)
{
    die($argv[0] . ": could not open `" . $argv[1] . "'.\n");
}

$dp = caca_create_display($cv);
if(! $dp)
{
    die("Can't create display\n");
}

caca_refresh_display($dp);

caca_get_event($dp, CACA_EVENT_KEY_PRESS, -1);

?>
