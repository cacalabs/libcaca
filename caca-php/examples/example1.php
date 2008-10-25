#!/usr/bin/php5
<?

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
if (!$canvas) {
	die("Error while creating main canvas\n");
}

$pig = caca_create_canvas(0, 0);
if (!$pig) {
	die("Error while creating canvas pig\n");
}

$display = caca_create_display($canvas);
if (!$display) {
	die("Error while attaching canvas to display\n");
}


caca_set_color_ansi($pig, CACA_LIGHTMAGENTA, CACA_TRANSPARENT);
caca_set_color_ansi($canvas, CACA_LIGHTBLUE, CACA_TRANSPARENT);
caca_import_memory($pig, $pig_str, "text");
caca_set_display_time($display, 30000);

$x = $y = 0;
$ix = $iy = 1;

while (caca_get_event($display, CACA_EVENT_KEY_PRESS)) {
	// In case of resize ...
	if ($x + caca_get_canvas_width($pig) - 1 >= caca_get_canvas_width($canvas) || $x < 0 )
		$x = 0;
	if ($y + caca_get_canvas_height($pig) - 1 >= caca_get_canvas_height($canvas) || $y < 0 )
		$y = 0;

	caca_clear_canvas($canvas);

	// Draw
	caca_blit($canvas, $x, $y, $pig);
        caca_put_str($canvas, caca_get_canvas_width($canvas) / 2 - 10, caca_get_canvas_height($canvas) / 2, "Powered by libcaca ".caca_get_version());
	caca_refresh_display($display);


	// Move cursor
	$x += $ix;
	$y += $iy;
	if ($x + caca_get_canvas_width($pig) >= caca_get_canvas_width($canvas) || $x < 0 )
		$ix = -$ix;
	if ($y + caca_get_canvas_height($pig) >= caca_get_canvas_height($canvas) || $y < 0 )
		$iy = -$iy;
}

