#!/usr/bin/php5
<?php
/*
 *  img2txt       image to text converter
 *  Copyright (c) 2008 Benjamin C. Wiley Sittler <bsittler@gmail.com>
 *
 *  This file is a Php port of "src/img2txt.c"
 *  which is:
 *  Copyright (c) 2006 Sam Hocevar <sam@hocevar.net>
 *                2007 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                All Rights Reserved
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
    fprintf(STDERR, "Usage: %s [OPTIONS]... <IMAGE>\n", $argv[0]);
    fprintf(STDERR, "Convert IMAGE to any text based available format.\n");
    fprintf(STDERR, "Example : %s -W 80 -f ansi ./caca.png\n\n", $argv[0]);
    fprintf(STDERR, "Options:\n");
    fprintf(STDERR, "  -h, --help\t\t\tThis help\n");
    fprintf(STDERR, "  -v, --version\t\t\tVersion of the program\n");
    fprintf(STDERR, "  -W, --width=WIDTH\t\tWidth of resulting image\n");
    fprintf(STDERR, "  -H, --height=HEIGHT\t\tHeight of resulting image\n");
    fprintf(STDERR, "  -x, --font-width=WIDTH\t\tWidth of output font\n");
    fprintf(STDERR, "  -y, --font-height=HEIGHT\t\tHeight of output font\n");
    fprintf(STDERR, "  -b, --brightness=BRIGHTNESS\tBrightness of resulting image\n");
    fprintf(STDERR, "  -c, --contrast=CONTRAST\tContrast of resulting image\n");
    fprintf(STDERR, "  -g, --gamma=GAMMA\t\tGamma of resulting image\n");
    fprintf(STDERR, "  -d, --dither=DITHER\t\tDithering algorithm to use :\n");
    $list = caca_get_dither_algorithm_list(caca_create_dither(imagecreate(1, 1)));
    foreach($list as $type => $name)
    {
        fprintf(STDERR, "\t\t\t%s: %s\n", $type, $name);
    }

    fprintf(STDERR, "  -f, --format=FORMAT\t\tFormat of the resulting image :\n");
    $list = caca_get_export_list();
    foreach($list as $type => $name)
    {
        fprintf(STDERR, "\t\t\t%s: %s\n", $type, $name);
    }
}

function version()
{
    printf(
    "img2txt Copyright 2006-2007 Sam Hocevar and Jean-Yves Lamoureux\n" .
    "        Copyright 2008 Benjamin C. Wiley Sittler\n" .
    "Internet: <sam@hocevar.net> <jylam@lnxscene.org>\n" .
    "          <bsittler@gmail.com> Version: %s\n" .
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
    $cols = 0;
    $lines = 0;
    $font_width = 6;
    $font_height = 10;
    $format = NULL;
    $dither = NULL;
    $gamma = $brightness = $contrast = -1.0;

    $long_options = array(
        "width:"    => 'W',
        "height:"    => 'H',
        "font-width:"    => 'x',
        "font-height:"    => 'y',
        "format:"    => 'f',
        "dither:"    => 'd',
        "gamma:"    => 'g',
        "brightness:"    => 'b',
        "contrast:"    => 'c',
        "help"        => 'h',
        "version"    => 'v'
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
        fprintf(STDERR, "%s", $argv[0] . ": " . $e->getMessage() . "\n");
        usage($argc, $argv);
        return 2;
    }

    if($argc != 2)
    {
        fprintf(STDERR, "%s: wrong argument count\n", $argv[0]);
        usage($argc, $argv);
        return 1;
    }

    $cv = caca_create_canvas(0, 0);
    if(!$cv)
    {
        fprintf(STDERR, "%s: unable to initialise libcaca\n", $argv[0]);
        return 1;
    }

    $i_str = file_get_contents($argv[$argc-1]);
    $i = $i_str ? imagecreatefromstring($i_str) : NULL;
    if(!$i)
    {
        fprintf(STDERR, "%s: unable to load %s\n", $argv[0], $argv[$argc-1]);
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
        fprintf(STDERR, "%s: Can't dither image with algorithm '%s'\n", $argv[0], $dither?$dither:"fstein");
        return -1;
    }

    if($brightness!=-1) caca_set_dither_brightness ($i_dither, $brightness);
    if($contrast!=-1) caca_set_dither_contrast ($i_dither, $contrast);
    if($gamma!=-1) caca_set_dither_gamma ($i_dither, $gamma);

    caca_dither_bitmap($cv, 0, 0, $cols, $lines, $i_dither, $i);

    $export = caca_export_string($cv, $format?$format:"ansi");
    if(!$export)
    {
        fprintf(STDERR, "%s: Can't export to format '%s'\n", $argv[0], $format?$format:"ansi");
        return -1;
    }
    else
    {
        echo $export;
    }

    return 0;
}
exit(main());
?>
