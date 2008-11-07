<?php
/*
 *  img2txt       image to text converter
 *  Copyright (c) 2008 Benjamin C. Wiley Sittler <bsittler@gmail.com>
 *
 *  This file is a Php port of "src/img2txt.c"
 *  which is: 
 *  Copyright (c) 2006 Sam Hocevar <sam@zoy.org>
 *                2007 Jean-Yves Lamoureux <jylam@lnxscene.org>
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

$img2txt_php = isset($_SERVER['SCRIPT_NAME'])
		?
		$_SERVER['SCRIPT_NAME']
		:
		'img2txt.php';

$argv = array(basename($img2txt_php));
$file = isset($_FILES['file']) ? $_FILES['file']['tmp_name'] : NULL;
$filename = isset($_FILES['file']) ? $_FILES['file']['name'] : NULL;
$args = NULL;
if(isset($_REQUEST['args']))
{
	$args = $_REQUEST['args'];
	if(strlen($args))
	{
		foreach(explode(' ', $args) as $arg)
		{
			$argv[] = $arg;
		}
	}
}
$argc = count($argv);
$stderr = '';
$stdout = '';

class MygetoptException extends Exception
{
}

$myoptind = 0;

function mygetopt($shortopts, $longopts)
{
	global $argc, $argv, $myoptind;
	if($myoptind < 0)
	{
		$myoptind = 0;
	}
	if(($myoptind + 1) >= $argc)
	{
		return NULL;
	}
	$myoptind ++;
	$nextarg = $argv[$myoptind];
	$ret = NULL;
	if((substr($nextarg, 0, 1) != '-')
   	||
   	($nextarg == '-'))
	{
		$myoptind = $argc - 1;
	}
	else
	{
		$skipopt = $myoptind;
		$skipopts = 1;
		if($nextarg == '--')
		{
			$myoptind = $argc - 1;
		}
		else
		{
			$opt = NULL;
			$arg = NULL;
			foreach($longopts as $longopt)
			{
				$optional = false;
				$hasarg = false;
				if(($longopt != '::') && substr($longopt, -2) == '::')
				{
					$optional = true;
					$hasarg = true;
					$longopt = substr($longopt, 0, -2);
				}
				else if(($longopt != ':') && substr($longopt, -1) == ':')
				{
					$optional = false;
					$hasarg = true;
					$longopt = substr($longopt, 0, -1);
				}
				if($nextarg == ('--' . $longopt))
				{
					$opt = '--' . $longopt;
					if($hasarg && ! $optional)
					{
						if(($myoptind + 1) < $argc)
						{
							$myoptind ++;
							$skipopts ++;
							$arg = $argv[$myoptind];
						}
						else
						{
							throw new MygetoptException("option \"$opt\" requires an argument");
						}
					}
					break;
				}
				else if(substr($nextarg, 0, strlen('--' . $longopt . '='))
						==
						('--' . $longopt . '='))
				{
					$opt = '--' . $longopt;
					$arg = substr($nextarg, strlen($opt . '='));
					if(! $hasarg)
					{
						throw new MygetoptException("option \"$opt\" does not allow an argument");
					}
					break;
				}
			}
			if($opt === NULL)
			{
				for($i = 0; $i < strlen($shortopts); $i ++)
				{
					$optional = false;
					$hasarg = false;
					$shortopt = substr($shortopts, $i, 1);
					if(substr($shortopts, $i + 1, 2) == '::')
					{
						$optional = true;
						$hasarg = true;
						$i += 2;
					}
					else if(substr($shortopts, $i + 1, 1) == ':')
					{
						$hasarg = true;
					}
					if($nextarg
   					==
   					('-' . $shortopt))
					{
						$opt = '-' . $shortopt;
						if($hasarg && ! $optional)
						{
							if(($myoptind + 1) < $argc)
							{
								$myoptind ++;
								$skipopts ++;
								$arg = $argv[$myoptind];
							}
							else
							{
								throw new MygetoptException("option \"$opt\" requires an argument");
							}
						}
						break;
					}
					else if(substr($nextarg, 0, strlen('-' . $shortopt))
							==
							('-' . $shortopt))
					{
						$opt = '-' . $shortopt;
						if($hasarg)
						{
							$arg = substr($nextarg, strlen($opt));
						}
						else
						{
							$argv[$myoptind] = '-' . substr($nextarg, strlen($opt));
							$myoptind --;
							$skipopts = 0;
						}
					}
				}
			}
			if($opt === NULL)
			{
				if(substr($nextarg, 0, strlen('--')) == '--')
				{
					$longopt = substr($nextarg, strlen('--'));
					if(strpos($longopt, '=') !== false)
					{
						$longopt = substr($longopt, 0, strpos($longopt, '='));
					}
					throw new MygetoptException("option \"--$longopt\" is not recognized");
				}
				$shortopt = substr($nextarg, strlen('-'), 1);
				throw new MygetoptException("option \"-$shortopt\" is not recognized");
			}
			$ret = array($opt, $arg);
		}
		if ($skipopts > 0)
		{
			for($i = 0; $i < $argc; $i ++)
			{
				if(($i < $skipopt) || ($i >= ($skipopt + $skipopts)))
				{
					$new_argv[] = $argv[$i];
				}
			}
			if($myoptind >= $skipopt)
			{
				$myoptind -= $skipopts;
			}
			$argv = $new_argv;
			$argc = count($argv);
		}
	}
	return $ret;
}

function usage($argc, $argv)
{
	global $stderr;
	$stderr .= sprintf("Usage: %s [OPTIONS]... <IMAGE>\n", $argv[0]);
	$stderr .= sprintf("Convert IMAGE to any text based available format.\n");
	$stderr .= sprintf("Example : -W 80 -f html logo-caca.png\n\n", $argv[0]);
	$stderr .= sprintf("Options:\n");
	$stderr .= sprintf("  -h, --help\t\t\tThis help\n");
	$stderr .= sprintf("  -v, --version\t\t\tVersion of the program\n");
	$stderr .= sprintf("  -W, --width=WIDTH\t\tWidth of resulting image\n");
	$stderr .= sprintf("  -H, --height=HEIGHT\t\tHeight of resulting image\n");
	$stderr .= sprintf("  -x, --font-width=WIDTH\t\tWidth of output font\n");
	$stderr .= sprintf("  -y, --font-height=HEIGHT\t\tHeight of output font\n");
	$stderr .= sprintf("  -b, --brightness=BRIGHTNESS\tBrightness of resulting image\n");
	$stderr .= sprintf("  -c, --contrast=CONTRAST\tContrast of resulting image\n");
	$stderr .= sprintf("  -g, --gamma=GAMMA\t\tGamma of resulting image\n");
	$stderr .= sprintf("  -d, --dither=DITHER\t\tDithering algorithm to use :\n");
	$list = caca_get_dither_algorithm_list(caca_create_dither(imagecreate(1, 1)));
	foreach($list as $type => $name)
	{
		$stderr .= sprintf("\t\t\t%s: %s\n", $type, $name);
	}

	$stderr .= sprintf("  -f, --format=FORMAT\t\tFormat of the resulting image :\n");
	$list = caca_get_export_list();
	foreach($list as $type => $name)
	{
		$stderr .= sprintf("\t\t\t%s: %s\n", $type, $name);
	}
}

function version()
{
	global $stdout;
	$stdout .= sprintf(
	"img2txt Copyright 2006-2007 Sam Hocevar and Jean-Yves Lamoureux\n" .
	"        Copyright 2008 Benjamin C. Wiley Sittler\n" .
	"Internet: <sam@zoy.org> <jylam@lnxscene.org> <bsittler@gmail.com> Version: %s\n" .
	"\n" .
	"img2txt, along with its documentation, may be freely copied and distributed.\n" .
	"\n" .
	"The latest version of img2txt is available from the web site,\n" .
	"        http://caca.zoy.org/wiki/libcaca in the libcaca package.\n" .
	"\n",
	caca_get_version());
}
function main()
{
	global $argc, $argv;
	global $stderr;
	$cols = 0;
	$lines = 0;
	$font_width = 6;
	$font_height = 10;
	$format = NULL;
	$dither = NULL;
	$gamma = $brightness = $contrast = -1.0;

	$long_options = array(
		"width:"	=> 'W',
		"height:"	=> 'H',
		"font-width:"	=> 'x',
		"font-height:"	=> 'y',
		"format:"	=> 'f',
		"dither:"	=> 'd',
		"gamma:"	=> 'g',
		"brightness:"	=> 'b',
		"contrast:"	=> 'c',
		"help"		=> 'h',
		"version"	=> 'v'
		);

	try {
		while($opt_and_arg = mygetopt("W:H:f:d:g:b:c:hvx:y:", array_keys($long_options)))
		{
			$opt = $opt_and_arg[0];
			$arg = $opt_and_arg[1];
			if((substr($opt, 0, 2) == '--')
				&&
				array_key_exists(substr($opt, strlen('--')) . (($arg !== NULL) ? ':' : ''), $long_options))
			{
				$opt = '-' . $long_options[substr($opt, strlen('--')) . (($arg !== NULL) ? ':' : '')];
			}
			switch($opt)
			{
			case '-W': /* --width */
				$cols = intval($arg);
				break;
			case '-H': /* --height */
				$lines = intval($arg);
				break;
			case '-x': /* --width */
				$font_width = intval($arg);
				break;
			case '-y': /* --height */
				$font_height = intval($arg);
				break;
			case '-f': /* --format */
				$format = $arg;
				break;
			case '-d': /* --dither */
				$dither = $arg;
				break;
			case '-g': /* --gamma */
				$gamma = floatval($arg);
				break;
			case '-b': /* --brightness */
				$brightness = floatval($arg);
				break;
			case '-c': /* --contrast */
				$contrast = floatval($arg);
				break;
			case '-h': /* --help */
				usage($argc, $argv);
				return 0;
			case '-v': /* --version */
				version();
				return 0;
			default:
				return 1;
			}
		}
	}
	catch (MygetoptException $e)
	{
		$stderr .= $argv[0] . ": " . $e->getMessage() . "\n";
		usage($argc, $argv);
		return 2;
	}

	global $file, $filename;

	if((! $file) && ($argc == 2) && ($argv[1] == 'logo-caca.png'))
	{
		$file = 'logo-caca.png';
		$argc = 1;
	}

	if($argc > 1)
	{
		$stderr .= sprintf("%s: too many arguments\n", $argv[0]);
		usage($argc, $argv);
		return 1;
	}

	if(! $file)
	{
		$stderr .= sprintf("%s: no image was provided\n", $argv[0]);
		usage($argc, $argv);
		return 1;
	}

	$cv = caca_create_canvas(0, 0);
	if(!$cv)
	{
		$stderr .= sprintf("%s: unable to initialise libcaca\n", $argv[0]);
		return 1;
	}

	$i_str = $file ? file_get_contents($file) : NULL;
	$i = $i_str ? imagecreatefromstring($i_str) : NULL;
	if(!$i)
	{
		$stderr .= sprintf("%s: unable to load %s\n", $argv[0], $filename);
		return 1;
	}

	/* Assume a 6×10 font */
	if(!$cols && !$lines)
	{
		$cols = 60;
		$lines = $cols * imagesy($i) * $font_width / imagesx($i) / $font_height;
	}
	else if($cols && !$lines)
	{
		$lines = $cols * imagesy($i) * $font_width / imagesx($i) / $font_height;
	}
	else if(!$cols && $lines)
	{
		$cols = $lines * imagesx($i) * $font_height / imagesy($i) / $font_width;
	}


	caca_set_canvas_size($cv, $cols, $lines);
	caca_set_color_ansi($cv, CACA_DEFAULT, CACA_TRANSPARENT);
	caca_clear_canvas($cv);
	$i_dither = caca_create_dither($i);
	if(! caca_set_dither_algorithm($i_dither, $dither?$dither:"fstein"))
	{
		$stderr .= sprintf("%s: Can't dither image with algorithm '%s'\n", $argv[0], $dither?$dither:"fstein");
		return -1;
	}

	if($brightness!=-1) caca_set_dither_brightness ($i_dither, $brightness);
	if($contrast!=-1) caca_set_dither_contrast ($i_dither, $contrast);
	if($gamma!=-1) caca_set_dither_gamma ($i_dither, $gamma);

	caca_dither_bitmap($cv, 0, 0, $cols, $lines, $i_dither, $i);

	$format = $format ? $format : 'html';

	$export = caca_export_string($cv, $format);
	if(!$export)
	{
		$stderr .= sprintf("%s: Can't export to format '%s'\n", $argv[0], $format);
		return -1;
	}
	else
	{
		$content_type_map = array(
			'ansi' => 'text/plain; charset=CP437',
			'utf8' => 'text/plain; charset=UTF-8',
			'utf8cr' => 'text/plain; charset=UTF-8',
			'html' => 'text/html; charset=UTF-8',
			'html3' => 'text/html; charset=UTF-8',
			'bbfr' => 'text/plain; charset=UTF-8',
			'irc' => 'text/plain; charset=UTF-8',
			'ps' => 'application/postscript',
			'svg' => 'image/svg+xml',
			'tga' => 'image/x-targa'
			);

		$download_extension_map = array(
			'caca' => 'caca',
			'ansi' => 'txt',
			'utf8' => 'txt',
			'utf8cr' => 'txt',
			'irc' => 'txt',
			'tga' => 'tga'
			);

		$inline_extension_map = array(
			'bbfr' => 'txt',
			'ps' => 'ps',
			'svg' => 'svg'
			);

		if (! array_key_exists($format, $content_type_map))
			$content_type = 'application/octet-stream';
		else
			$content_type = $content_type_map[$format];

		header('Content-Type: ' . $content_type);
		if (array_key_exists($format, $download_extension_map))
			header('Content-Disposition: attachment; filename=export.' . $download_extension_map[$format]);
		else if (array_key_exists($format, $inline_extension_map))
			header('Content-Disposition: inline; filename=export.' . $inline_extension_map[$format]);

		echo $export;
	}

	return 0;
}
$ret = 1;
if(isset($_REQUEST['args']) || $file)
{
	$ret = main();
}
if($ret || strlen($stdout) || strlen($stderr))
{
	header('Content-Type: text/html; charset=UTF-8');
	?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
<title>image to text converter</title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
</head>
<body>
<form id="img2txtform" name="img2txtform" action="#" enctype="multipart/form-data" method="post">
<label for="file">Image:</label>
<input id="file" name="file" type="file" />
<br />
<label for="args">Options:</label>
<input id="args" name="args" type="text" value="<?= isset($_REQUEST['args']) ? htmlspecialchars($_REQUEST['args']) : '' ?>" size="80" />
<br />
<input type="submit" />
</form>
<?php
		;
	if(strlen($stderr))
	{
		?><pre xml:space="preserve"><em><?= preg_replace('!(logo-caca[.]png)!', '<a href="$1">$1</a>', htmlspecialchars($stderr)) ?></em></pre><?php
			;
	}
	if(strlen($stdout))
	{
		?><pre xml:space="preserve"><?= preg_replace('!([&]lt;)([.a-zA-Z0-9]+[@])([-.a-zA-Z0-9]+)([&]gt;)!', '$1<a href="mailto:$2$3">$2...</a>$4', preg_replace('!(\s|^)(https?://[-.:_/0-9a-zA-Z%?=&;#]+)(\s|$)!', '$1<a href="$2">$2</a>$3', htmlspecialchars($stdout))) ?></pre><?php
			;
	}
	?>
</body>
</html>
<?php
		;
}
?>
