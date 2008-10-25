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


caca_set_color_ansi($pig, CACA_LIGHTRED, CACA_WHITE);
caca_import_memory($pig, $pig_str, "text");
caca_blit($canvas, caca_get_canvas_width($canvas) / 2 - 20, caca_get_canvas_height($canvas) /2 - 7, $pig);
caca_refresh_display($display);

sleep(5);


