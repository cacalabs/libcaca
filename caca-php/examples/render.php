#!/usr/bin/php5
<?

if (php_sapi_name() != "cli") {
    die("You have to run this program with php-cli!\n");
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
echo caca_export_string($canvas, "utf8");

$font = caca_load_builtin_font("Monospace Bold 12");

$width = caca_get_canvas_width($canvas) * caca_get_font_width($font);
$height = caca_get_canvas_height($canvas) * caca_get_font_height($font);

$img = imagecreatetruecolor($width, $height);

caca_render_canvas($canvas, $font, $img);

imagepng($img, "out.png");
echo "Please, open new created png file: out.png\n";

?>
