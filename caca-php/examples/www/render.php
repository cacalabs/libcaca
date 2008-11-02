<?php

function pig() {
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
	return $canvas;
}


if (isset($_GET["png"])) {
	$canvas = pig();
	$font = caca_load_builtin_font("Monospace Bold 12");
	$width = caca_get_canvas_width($canvas) * caca_get_font_width($font);
	$height = caca_get_canvas_height($canvas) * caca_get_font_height($font);
	$img = imagecreatetruecolor($width, $height);
	caca_render_canvas($canvas, $font, $img);
	header("Content-type: image/png");
	imagepng($img);
}
else {
?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
<title>Я люблю Либкаку</title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
<link rel="StyleSheet" href="caca-php.css" type="text/css" />
</head>
<body text="silver" bgcolor="black">

<h1>Text mode:</h1>
<?echo caca_export_string(pig(), "html3");?>

<h1>Generated image:</h1>
<a href="render.php?png=1"><img alt="[PNG]" src="render.php?png=1" border="0" /></a>
</body>
</html>

<?php
}

?>
