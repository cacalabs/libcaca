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
caca_set_color_ansi($canvas, CACA_RED, CACA_WHITE);
caca_import_string($canvas, $pig_str, "text");
caca_set_color_ansi($canvas, CACA_BLUE, CACA_LIGHTGRAY);
caca_put_str($canvas, 0, 0, "Я люблю Либкаку");
echo caca_export_string($canvas, "utf8");

$img = imagecreatetruecolor(400, 300);
$font = caca_load_builtin_font("Monospace Bold 12");
caca_render_canvas($canvas, $font, $img);

imagepng($img, "out.png");
echo "Please, open new created png file: out.png\n";

?>
