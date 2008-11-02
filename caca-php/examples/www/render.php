<?php
$pngmode = isset($_REQUEST['png']) ? ($_REQUEST['png'] != '') : false;

if ($pngmode)
    header('Content-Type: image/png');
else
    header('Content-Type: text/html; charset=UTF-8');

if (! $pngmode)
{
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
<title>Я люблю Либкаку</title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
<link rel="StyleSheet" href="caca-php.css" type="text/css" />
</head>
<body text="silver" bgcolor="black">
<?php
        ;
}
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

$canvas = caca_create_canvas(0, 0);
caca_set_color_ansi($canvas, CACA_RED, CACA_WHITE);
caca_import_string($canvas, $pig_str, "text");
caca_set_color_ansi($canvas, CACA_BLUE, CACA_LIGHTGRAY);
caca_put_str($canvas, 0, 0, "Я люблю Либкаку");
if (! $pngmode)
    echo caca_export_string($canvas, "html3");

$font = caca_load_builtin_font("Monospace Bold 12");

$width = caca_get_canvas_width($canvas) * caca_get_font_width($font);
$height = caca_get_canvas_height($canvas) * caca_get_font_height($font);

$img = imagecreatetruecolor($width, $height);

caca_render_canvas($canvas, $font, $img);

if ($pngmode)
    imagepng($img);
else
{
?>
<form action="#">
<label for="png">Please, open new created png file:</label>
<input type="submit" id="png" name="png" value="Display" />
</form>
</body>
</html>
<?php
;
}