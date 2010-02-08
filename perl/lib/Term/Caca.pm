package Term::Caca;

require Exporter;
require DynaLoader;
$VERSION = '0.9_1';
@ISA = qw(Exporter DynaLoader);
Term::Caca->bootstrap($VERSION);

use strict;
use Term::Caca::Constants ':all';

# Basic functions

# constructor
sub new {
  my ($class) = @_;
  _init();
  my $self = { };
  return bless($self => $class);
}
*init = \*new;

# set delay for establishing constant framerate
sub set_delay {
  my ($self, $usec) = @_;
  $usec ||= 0;
  _set_delay($usec);
}

#
sub get_feature {
  my ($self, $feature) = @_;
  $feature ||= 0;
  return _get_feature($feature);
}

#
sub set_feature {
  my ($self, $feature) = @_;
  $feature ||= 0;
  _get_feature($feature);
}

#
sub get_feature_name {
  my ($self, $feature) = @_;
  $feature ||= 0;
  return _get_feature_name($feature);
}

#
sub get_rendertime {
# my ($self) = @_;
  return _get_rendertime();
}

#
sub get_width {
# my ($self) = @_;
  return _get_width();
}

#
sub get_height {
# my ($self) = @_;
  return _get_height();
}

#
sub set_window_title {
  my ($self, $title) = @_;
  $title ||= "";
  return _set_window_title($title);
}

#
sub get_window_width {
# my ($self) = @_;
  return _get_window_width();
}

#
sub get_window_height {
# my ($self) = @_;
  return _get_window_height();
}

#
sub refresh {
  _refresh();
}

# destructor
sub DESTROY {
  my ($self) = @_;
  _end();
}

# Event handling

#
sub get_event {
  my ($self, $event_mask) = @_;
  if (!defined($event_mask)) {
    $event_mask = 0xFFFFFFFF;
  }
  return _get_event($event_mask);
}

#
sub get_mouse_x {
  my ($self) = @_;
  return _get_mouse_x();
}

#
sub get_mouse_y {
  my ($self) = @_;
  return _get_mouse_y();
}

#
sub wait_event {
  my ($self, $event_mask) = @_;
  $event_mask ||= CACA_EVENT_ANY;
  return _wait_event($event_mask);
}

1;

# Character printing

#
sub set_color {
  my ($self, $fgcolor, $bgcolor) = @_;
  $fgcolor ||= CACA_COLOR_LIGHTGRAY;
  $bgcolor ||= CACA_COLOR_BLACK;
  return _set_color($fgcolor, $bgcolor);
}

#
sub get_fg_color {
  my ($self) = @_;
  return _get_fg_color();
}

#
sub get_bg_color {
  my ($self) = @_;
  return _get_bg_color();
}

#
sub get_color_name {
  my ($self, $color) = @_;
  return undef unless(defined($color));
  return _get_color_name($color);
}

#
sub putchar {
  my ($self, $x, $y, $c) = @_;
  $x ||= 0;
  $y ||= 0;
  $c ||= "";
  _putchar($x, $y, $c);
}

#
sub putstr {
  my ($self, $x, $y, $s) = @_;
  $x ||= 0;
  $y ||= 0;
  $s ||= "";
  _putstr($x, $y, $s);
}

# faking it by doing printf on the perl side
sub printf {
  my ($self, $x, $y, $s, @args) = @_;
  $x ||= 0;
  $y ||= 0;
  my $string = sprintf($s, @args);
  _putstr($x, $y, $string);
}

#
sub clear {
  _clear();
}

# Primitives drawing

#
sub draw_line {
  my ($self, $x1, $y1, $x2, $y2, $c) = @_;
  _draw_line($x1, $y1, $x2, $y2, $c);
}

#
sub draw_polyline {
  my ($self, $x, $y, $n, $c) = @_;
  _draw_polyline($x, $y, $n, $c);
}

#
sub draw_thin_line {
  my ($self, $x1, $y1, $x2, $y2) = @_;
  _draw_thin_line($x1, $y1, $x2, $y2);
}

#
sub draw_thin_polyline {
  my ($self, $x, $y, $n) = @_;
  _draw_thin_polyline($x, $y, $n);
}

#
sub draw_circle {
  my ($self, $x, $y, $r, $c) = @_;
  # TODO : check for sane values
  _draw_circle($x, $y, $r, $c);
}

#
sub draw_ellipse {
  my ($self, $x0, $y0, $ra, $rb, $c) = @_;
  _draw_ellipse($x0, $y0, $ra, $rb, $c);
}

#
sub draw_thin_ellipse {
  my ($self, $x0, $y0, $ra, $rb) = @_;
  _draw_ellipse($x0, $y0, $ra, $rb);
}

#
sub fill_ellipse {
  my ($self, $x0, $y0, $ra, $rb, $c) = @_;
  _fill_ellipse($x0, $y0, $ra, $rb, $c);
}

#
sub draw_box {
  my ($self, $x0, $y0, $x1, $y1, $c) = @_;
  _draw_box($x0, $y0, $x1, $y1, $c);
}

#
sub draw_thin_box {
  my ($self, $x0, $y0, $x1, $y1) = @_;
  _draw_thin_box($x0, $y0, $x1, $y1);
}

#
sub fill_box {
  my ($self, $x0, $y0, $x1, $y1, $c) = @_;
  _fill_box($x0, $y0, $x1, $y1, $c);
}

#
sub draw_triangle {
  my ($self, $x0, $y0, $x1, $y1, $x2, $y2, $c) = @_;
  _draw_triangle($x0, $y0, $x1, $y1, $x2, $y2, $c);
}

#
sub draw_thin_triangle {
  my ($self, $x0, $y0, $x1, $y1, $x2, $y2) = @_;
  _draw_thin_triangle($x0, $y0, $x1, $y1, $x2, $y2);
}

#
sub fill_triangle {
  my ($self, $x0, $y0, $x1, $y1, $x2, $y2, $c) = @_;
  _fill_triangle($x0, $y0, $x1, $y1, $x2, $y2, $c);
}

# Mathematical functions

#
sub rand {
  my ($self, $min, $max) = @_;
  return _rand($min, $max);
}

#
sub sqrt {
  my ($self, $n) = @_;
  return _sqrt($n);
}

# Sprite handling

#
sub load_sprite {
  my ($self, $file) = @_;
  my $sprite = _load_sprite($file);
}

#
sub get_sprite_frames {
  my ($self, $sprite) = @_;
  return _get_sprite_frames($sprite);
}

#
sub get_sprite_width {
  my ($self, $sprite) = @_;
  return _get_sprite_width($sprite);
}

#
sub get_sprite_height {
  my ($self, $sprite) = @_;
  return _get_sprite_height($sprite);
}

#
sub get_sprite_dx {
  my ($self, $sprite) = @_;
  return _get_sprite_dx($sprite);
}

#
sub get_sprite_dy {
  my ($self, $sprite) = @_;
  return _get_sprite_dy($sprite);
}

#
sub draw_sprite {
  my ($self, $x, $y, $sprite, $f) = @_;
  _draw_sprite($x, $y, $sprite, $f);
}

#
sub free_sprite {
  my ($self, $sprite) = @_;
  _free_sprite($sprite);
}

# Bitmap handling

#
sub create_bitmap {
  my ($self, $bpp, $w, $h, $pitch, $rmask, $gmask, $bmask, $amask) = @_;
  _create_bitmap($bpp, $w, $h, $pitch, $rmask, $gmask, $bmask, $amask);
}

#
sub set_bitmap_palette {
  my ($self, $bitmap, $red, $green, $blue, $alpha) = @_;
  _set_bitmap_palette($bitmap, $red, $green, $blue, $alpha);
}

#
sub draw_bitmap {
  my ($self, $x1, $y1, $x2, $y2, $bitmap, $pixels) = @_;
  _draw_bitmap($x1, $y1, $x2, $y2, $bitmap, $pixels);
}

sub free_bitmap {
  my ($self, $bitmap) = @_;
  _free_bitmap($bitmap);
}

__END__

=head1 NAME

Term::Caca - perl interface for libcaca (Colour AsCii Art library)

=head1 SYNOPSIS

=head1 DESCRIPTION

=head2 Class Methods

=head2 Object Methods

=head1 AUTHOR

=head1 SEE ALSO

=cut

# vim:sw=2 sts=2 expandtab
