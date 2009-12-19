#!/usr/bin/php5
<?php
  /*
   *  Test          PHP bindings test program
   *  Copyright (c) 2008 Benjamin C. Wiley Sittler <bsittler@gmail.com>
   *
   *  This file is a Php port of "caca-sharp/test.cs"
   *  which is:
   *  Copyright (c) 2006 Jean-Yves Lamoureux <jylam@lnxscene.org>
   *                2007 Sam Hocevar <sam@hocevar.net>
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

if (php_sapi_name() != "cli") {
	die("You have to run this program with php-cli!\n");
}

include dirname($argv[0]) . '/../caca.php';

class DemoCanvas extends Canvas
{
	private $image;

	private $startTime;
	private $d;
	private $scroll;

	function __construct()
	{
		parent::__construct();
		$this->startTime = gettimeofday(true);

		$message = " --- POWERED BY LIBCACA --- OLDSCHOOL TEXT EFFECTS ARE 100% PURE WIN";

		$this->scroll = new Canvas(strlen($message), 1);
		$this->scroll->setColorAnsi(AnsiColor::WHITE, AnsiColor::TRANSPARENT);
		$this->scroll->putStr(0, 0, $message);

		$fontList = Font::getList();
		$f = new Font($fontList[1]);
		$w = $f->getWidth() * strlen($message);
		$h = $f->getHeight();
		$this->image = imagecreatetruecolor($w, $h);
		imagealphablending($this->image, false);
		imagesavealpha($this->image, true);
		$this->d = new Dither($this->image);
		$f->Render($this->scroll, $this->image);
	}

	function Draw()
	{
		$barCount = 6;
		$t = (gettimeofday(true) - $this->startTime) * 1000;

		$this->Clear();

		$this->setColorAnsi(AnsiColor::WHITE, AnsiColor::BLACK);
		for($i = 0; $i < $barCount; $i++)
		{
			$v = ((sin(($t / 500.0)
					   + (1.0 * $i / $barCount)) + 1) / 2) * $this->getHeight();
			$p1_x = 0; $p1_y = intval($v);
			$p2_x = $this->getWidth() - 1; $p2_y = intval($v);

			$this->setColorAnsi(($i + 9), AnsiColor::BLACK);
			/* drawLine is already clipped, we don't care about overflows */
			$this->drawLine($p1_x + 0, $p1_y - 2, $p2_x + 0, $p2_y - 2, ord('-'));
			$this->drawLine($p1_x + 0, $p1_y - 1, $p2_x + 0, $p2_y - 1, ord('*'));
			$this->drawLine($p1_x,     $p1_y,     $p2_x,     $p2_y,     ord('#'));
			$this->drawLine($p1_x + 0, $p1_y + 1, $p2_x + 0, $p2_y + 1, ord('*'));
			$this->drawLine($p1_x + 0, $p1_y + 2, $p2_x + 0, $p2_y + 2, ord('-'));
		}

		$w = $this->getWidth();
		$h = $this->getHeight();
		$x = intval(($t / 10) % (12 * $w));
		$y = intval($h * (2.0 + sin($t / 200.0)) / 4);
		$this->d->bitmap($this, - $x, $h / 2 - $y, $w * 12, $y * 2);
		$this->d->bitmap($this, 12 * $w - $x, $h / 2 - $y, $w * 12, $y * 2);

		$this->setColorAnsi(AnsiColor::WHITE, AnsiColor::BLUE);
		$this->putStr($this->getWidth() - 30, $this->getHeight() - 2, " -=[ Powered by libcaca ]=- ");
		$this->setColorAnsi(AnsiColor::WHITE, AnsiColor::BLACK);
	}
}

class DemoDisplay extends Display
{
	private $cv;

	function __construct(DemoCanvas $_cv)
	{
		parent::__construct($_cv);
		$this->setTitle("libcaca PHP Bindings test suite");
		$this->setDisplayTime(20000); // Refresh every 20 ms
		$this->cv = $_cv;
	}

	function EventLoop()
	{
		while(! ($ev = $this->getEvent(EventType::KEY_RELEASE, 10)))
		{
			$this->cv->Draw();

			$this->Refresh();
		}

		if($ev->getKeyCh() > 0x20 && $ev->getKeyCh() < 0x7f)
			printf("Key pressed: %c\n", $ev->getKeyCh());
		else
			printf("Key pressed: 0x%x\n", $ev->getKeyCh());
	}
}

class Test
{
	static function Main()
	{
		printf("libcaca %s PHP test\n", Libcaca::getVersion());
		printf("(c) 2006 Jean-Yves Lamoureux <jylam@lnxscene.org>\n");
		printf("(c) 2007 Sam Hocevar <sam@hocevar.net>\n");
		printf("(c) 2008 Benjamin C. Wiley Sittler <bsittler@gmail.com>\n");

		/* Instanciate a caca canvas */
		$cv = new DemoCanvas();

		/* We have a proper canvas, let's display it using Caca */
		$dp = new DemoDisplay($cv);

		/* Random number. This is a static method,
		 not to be used with previous instance */
		printf("A random number: %d\n", Libcaca::Rand(0, 1337));

		$dp->EventLoop();
	}
}

Test::Main();

?>
