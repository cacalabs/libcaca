
if USE_PYTHON
cacadir = $(pythondir)/caca
caca_PYTHON = \
    caca/__init__.py \
    caca/canvas.py \
    caca/common.py \
    caca/display.py \
    caca/dither.py \
    caca/font.py
endif

EXTRA_DIST = \
    setup.py \
    examples/blit.py \
    examples/cacainfo.py \
    examples/colors.py \
    examples/drawing.py \
    examples/driver.py \
    examples/event.py \
    examples/figfont.py \
    examples/frames.py \
    examples/img2txt.py \
    examples/gol.py \
    examples/text.py

echo-sources: ; echo $(caca_PYTHON)

