#!/usr/bin/php5
<?php
/*
 *  cacapig.php      sample program for libcaca php binding
 *  Copyright (c) 2008 Nicolas Vion <nico@yojik.eu>
 *
 *  This file is a Php port of "cxx/cxxtest.cpp"
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

include dirname($argv[0]) . '/../caca.php';

$pig_str = <<<EOT

    _._ _..._ .-',     _.._(`))
   '-. `     '  /-._.-'    ',/
      )         \            '.
     / _    _    |             \
    |  a    a    /   PHP        |
    \   .-.                     ;
     '-('' ).-'       ,'       ;
        '-;           |      .'
           \           \    /
           | 7  .__  _.-\   \
           | |  |  ``/  /`  /
      jgs /,_|  |   /,_/   /
             /,_/      '`-'
EOT;

$canvas = new Canvas();
if (!$canvas) {
    die("Error while creating main canvas\n");
}

$pig = new Canvas();
if (!$pig) {
    die("Error while creating canvas pig\n");
}

$display = new Display($canvas);
if (!$display) {
    die("Error while attaching canvas to display\n");
}


$pig->setColorANSI(AnsiColor::LIGHTMAGENTA, AnsiColor::TRANSPARENT);
$pig->importString($pig_str, "text");
$display->setDisplayTime(20000);

$x = $y = 0;
$ix = $iy = 1;

while (! $display->getEvent(EventType::KEY_PRESS)) {
    // In case of resize ...
    if ($x + $pig->getWidth() - 1 >= $canvas->getWidth() || $x < 0 )
        $x = 0;
    if ($y + $pig->getHeight() - 1 >= $canvas->getHeight() || $y < 0 )
        $y = 0;

    $canvas->Clear();

    // Draw
    $canvas->Blit($x, $y, $pig, NULL);
    $canvas->setColorANSI(AnsiColor::LIGHTBLUE, AnsiColor::BLACK);
    $canvas->putStr($canvas->getWidth() / 2 - 10, $canvas->getHeight() / 2, "Powered by libcaca ".Libcaca::getVersion());
    $display->refresh();


    // Move cursor
    $x += $ix;
    $y += $iy;
    if ($x + $pig->getWidth() >= $canvas->getWidth() || $x < 0 )
        $ix = -$ix;
    if ($y + $pig->getHeight() >= $canvas->getHeight() || $y < 0 )
        $iy = -$iy;
}

?>
