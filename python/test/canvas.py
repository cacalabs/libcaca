# -*- coding: utf8 -*-
#
# libcaca            Colour ASCII Art library
#                    Python language bindings
# Copyright (c) 2012 Alex Foulon <alxf@lavabit.com>
#                    All Rights Reserved
#
# This program is free software. It comes without any warranty, to
# the extent permitted by applicable law. You can redistribute it
# and/or modify it under the terms of the Do What the Fuck You Want
# to Public License, Version 2, as published by Sam Hocevar. See
# http://www.wtfpl.net/ for more details.
#

""" Libcaca canvas test suite """

import unittest

from caca.canvas import Canvas, CanvasError

class CanvasTestCase(unittest.TestCase):
    """ Class to test canvas functions.
    """
    def test_create(self):
        """ module canvas: Canvas.__init__()
        """
        self.assertTrue(isinstance(Canvas(), Canvas))
        self.assertTrue(isinstance(Canvas(10, 10), Canvas))
        self.assertRaises(CanvasError, Canvas, "a", 12)
        self.assertRaises(CanvasError, Canvas, 12, "a")

    def test_get_width(self):
        """ module canvas: Canvas.get_width()
        """
        cv = Canvas(5, 10)
        self.assertEqual(5, cv.get_width())

    def test_get_height(self):
        """ module canvas: Canvas.get_width()
        """
        cv = Canvas(5, 10)
        self.assertEqual(10, cv.get_height())

    def test_set_size(self):
        """ module canvas: Canvas.set_size()
        """
        cv = Canvas(0, 0)
        cv.set_size(1234, 1001)

        self.assertEqual((1234, 1001), (cv.get_width(), cv.get_height()))
        self.assertRaises(CanvasError, cv.set_size, "a", 12)
        self.assertRaises(CanvasError, cv.set_size, 12, "a")

    def test_get_char(self):
        """ module canvas: Canvas.get_char()
        """
        cv = Canvas(1, 1)
        self.assertEqual(" ", cv.get_char(0, 0))

    def test_put_char(self):
        """ module canvas: Canvas.put_char()
        """
        cv = Canvas(10, 1)
        self.assertEqual(1, cv.put_char(0, 0, "z"))
        self.assertEqual(1, cv.put_char(1, 0, "é"))
        self.assertEqual("z", cv.get_char(0, 0))
        self.assertEqual("é", cv.get_char(1, 0))
        self.assertRaises(CanvasError, cv.put_char, "a", 1, 2)
        self.assertRaises(CanvasError, cv.put_char, "a", 1, "b")

    def test_put_str(self):
        """ module canvas: Canvas.put_str()
        """
        cv = Canvas(10, 1)
        self.assertEqual(10, cv.put_str(0, 0, "teststring"))
        liststring = []
        for i in range(0, 10):
            liststring.append(cv.get_char(i, 0))

        self.assertEqual("teststring", "".join(liststring))
        self.assertRaises(CanvasError, cv.put_str, 0, 0, 111)
        self.assertRaises(CanvasError, cv.put_str, 0, "z", "abc")

    def test_printf(self):
        """ module canvas: Canvas.printf()
        """
        word1, word2 = "test", "string"
        cv = Canvas(10, 1)
        self.assertEqual(10, cv.printf(0, 0, "%s%s", word1, word2))
        liststring = []
        for i in range(0, 10):
            liststring.append(cv.get_char(i, 0))
        self.assertEqual("teststring", "".join(liststring))
        self.assertRaises(CanvasError, cv.printf, 0, 0, 111)
        self.assertRaises(CanvasError, cv.printf, 0, "z", "abc %s", "def")

    def test_wherex(self):
        """ module canvas: Canvas.wherex()
        """
        cv = Canvas(10, 10)
        self.assertEqual(0, cv.wherex())

    def test_wherey(self):
        """ module canvas: Canvas.wherey()
        """
        cv = Canvas(10, 10)
        self.assertEqual(0, cv.wherey())

    def test_gotoxy(self):
        """ module canvas: Canvas.gotoxy()
        """
        cv = Canvas(10, 10)
        cv.gotoxy(5, 5)
        self.assertEqual((5, 5), (cv.wherex(), cv.wherey()))
        self.assertRaises(CanvasError, cv.gotoxy, "a", 9)
        self.assertRaises(CanvasError, cv.gotoxy, 9, "a")

    def test_clear(self):
        """ module canvas: Canvas.clear()
        """
        cv = Canvas(5, 1)
        cv.put_str(0, 0, "test!")
        cv.clear()
        data = cv.export_to_memory("utf8").strip('\n')
        self.assertEqual("     ", data)

    def test_get_handle_x(self):
        """ module canvas: Canvas.get_handle_x()
        """
        cv = Canvas()
        self.assertEqual(0, cv.get_handle_x())

    def test_get_handle_y(self):
        """ module canvas: Canvas.get_handle_y()
        """
        cv = Canvas()
        self.assertEqual(0, cv.get_handle_y())

    def test_set_handle(self):
        """ module canvas: Canvas.set_handle()
        """
        cv = Canvas(10, 5)
        self.assertEqual(0, cv.set_handle(3, 3))
        self.assertEqual((3, 3), (cv.get_handle_x(), cv.get_handle_y()))
        self.assertRaises(CanvasError, cv.set_handle, "a", 1)
        self.assertRaises(CanvasError, cv.set_handle, 1, "a")

    def test_blit(self):
        """ module canvas: Canvas.blit()
        """
        cv1 = Canvas(10, 1)
        cv2 = Canvas(10, 1)
        cv2.put_str(0, 0, "teststring")
        self.assertEqual(0, cv1.blit(0, 0, cv2))
        self.assertEqual("teststring", cv1.export_to_memory("utf8").strip('\n'))
        self.assertRaises(CanvasError, cv1.blit, 0, 0, "abc")
        self.assertRaises(CanvasError, cv1.blit, "a", 0, cv2)

    def test_blit_mask(self):
        """ module canvas: Canvas.blit() (with mask)
        """
        cv1 = Canvas(10, 1)
        cv2 = Canvas(10, 1)
        mask = Canvas(10, 1)
        badm = Canvas()
        cv2.put_str(0, 0, "teststring")
        mask.put_str(0, 0, "####")
        self.assertEqual(0, cv1.blit(0, 0, cv2, mask))
        self.assertEqual("test      ", cv1.export_to_memory("utf8").strip("\n"))
        self.assertRaises(CanvasError, cv1.blit, 0, 0, cv2, badm)

    def test_set_boundaries(self):
        """ module canvas: Canvas.set_boundaries()
        """
        cv = Canvas()
        self.assertEqual(0, cv.set_boundaries(0, 0, 10, 10))
        self.assertEqual((10, 10), (cv.get_width(), cv.get_height()))
        self.assertEqual(0, cv.set_boundaries(0, 0, 5, 5))
        self.assertEqual((5, 5), (cv.get_width(), cv.get_height()))
        self.assertRaises(CanvasError, cv.set_boundaries, "a", 0, 10, 10)
        self.assertRaises(CanvasError, cv.set_boundaries, 0, 0, "abc", 10)

    def test_enable_dirty_rect(self):
        """ module canvas: Canvas.enable_dirty_rect()
        """
        cv = Canvas()
        self.assertRaises(CanvasError, cv.enable_dirty_rect)
        cv.disable_dirty_rect()
        self.assertEqual(0, cv.enable_dirty_rect())

    def test_add_dirty_rect(self):
        """ module canvas: Canvas.add_dirty_rect()
        """
        cv = Canvas(10, 10)
        self.assertEqual(0, cv.add_dirty_rect(4, 4, 2, 2))
        self.assertRaises(CanvasError, cv.add_dirty_rect, 11, 11, 20, 20)

    def test_remove_dirty_rect(self):
        """ module canvas: Canvas.remove_dirty_rect()
        """
        cv = Canvas(10, 10)
        cv.add_dirty_rect(4, 4, 2, 2)
        self.assertEqual(0, cv.remove_dirty_rect(4, 4, 2, 2))
        self.assertRaises(CanvasError, cv.remove_dirty_rect, 11, 11, 20, 20)

    def test_draw_line(self):
        """ module canvas: Canvas.draw_line()
        """
        cv = Canvas(10, 1)
        cv.draw_line(0, 0, 10, 1, "#")
        self.assertEqual("#" * 10, cv.export_to_memory("utf8").strip('\n'))
        self.assertRaises(CanvasError, cv.draw_line, 0, 0, 10, 1, 7)
        self.assertRaises(CanvasError, cv.draw_line, 0, "a", 10, 1, "#")

    def test_draw_thin_line(self):
        """ module canvas: Canvas.draw_thin_line()
        """
        cv = Canvas(10, 1)
        cv.draw_thin_line(0, 0, 10, 1)
        self.assertEqual("-" * 10, cv.export_to_memory("utf8").strip('\n'))
        self.assertRaises(CanvasError, cv.draw_thin_line, 0, "a", 10, 1)

    def test_draw_polyline(self):
        """ module canvas: Canvas.draw_polyline()
        """
        cv = Canvas(10, 10)
        self.assertEqual(0, cv.draw_polyline([(0, 0), (2, 2), (5, 2), (0, 0)],
                                              '#'))
        self.assertRaises(CanvasError, cv.draw_polyline, [], '#')
        self.assertRaises(CanvasError, cv.draw_polyline, [0, 1, 2, 4], '#')

    def test_draw_thin_polyline(self):
        """ module canvas: Canvas.draw_thin_polyline()
        """
        cv = Canvas(10, 10)
        self.assertEqual(0, cv.draw_thin_polyline([(0, 0), (2, 2),
                                                   (5, 2), (0, 0)]))
        self.assertRaises(CanvasError, cv.draw_thin_polyline, [])
        self.assertRaises(CanvasError, cv.draw_thin_polyline, [0, 1, 2, 4])

    def test_draw_circle(self):
        """ module canvas: Canvas.draw_circle()
        """
        cv = Canvas(10, 10)
        self.assertEqual(0, cv.draw_circle(5, 5, 3, '#'))
        self.assertRaises(CanvasError, cv.draw_circle, 0, 0, '#', '#')
        self.assertRaises(CanvasError, cv.draw_circle, 0, 0, 3, 0)

    def test_draw_ellipse(self):
        """ module canvas: Canvas.draw_ellipse()
        """
        cv = Canvas(10, 10)
        self.assertEqual(0, cv.draw_ellipse(5, 5, 3, 4, '#'))
        self.assertRaises(CanvasError, cv.draw_ellipse, 0, 0, 3, '#', '#')
        self.assertRaises(CanvasError, cv.draw_ellipse, 0, 0, 3, 5, 0)

    def test_draw_thin_ellipse(self):
        """ module canvas: Canvas.draw_thin_ellipse()
        """
        cv = Canvas(10, 10)
        self.assertEqual(0, cv.draw_thin_ellipse(5, 5, 3, 4))
        self.assertRaises(CanvasError, cv.draw_thin_ellipse, 0, 0, 3, '#')

    def test_fill_ellipse(self):
        """ module canvas: Canvas.fill_ellipse()
        """
        cv = Canvas(10, 10)
        self.assertEqual(0, cv.fill_ellipse(5, 5, 3, 3, '#'))
        self.assertRaises(CanvasError, cv.fill_ellipse, 0, 0, 3, '#', '#')
        self.assertRaises(CanvasError, cv.fill_ellipse, 0, 0, 3, 3, 0)

    def test_draw_box(self):
        """ module canvas: Canvas.draw_box()
        """
        cv = Canvas(10, 10)
        self.assertEqual(0, cv.draw_box(0, 0, 2, 2, '#'))
        self.assertRaises(CanvasError, cv.draw_box, 0, 0, 2, '#', '#')
        self.assertRaises(CanvasError, cv.draw_box, 0, 0, 2, 2, 0)

    def test_draw_thin_box(self):
        """ module canvas: Canvas.draw_thin_box()
        """
        cv = Canvas(10, 10)
        self.assertEqual(0, cv.draw_thin_box(0, 0, 2, 2))
        self.assertRaises(CanvasError, cv.draw_thin_box, 0, 0, 2, '#')

    def test_draw_cp437_box(self):
        """ module canvas: Canvas.draw_cp437_box()
        """
        cv = Canvas(10, 10)
        self.assertEqual(0, cv.draw_cp437_box(0, 0, 2, 2))
        self.assertRaises(CanvasError, cv.draw_cp437_box, 0, 0, 2, '#')

    def test_fill_box(self):
        """ module canvas: Canvas.fill_box()
        """
        cv = Canvas(10, 10)
        self.assertEqual(0, cv.fill_box(0, 0, 2, 2, '#'))
        self.assertRaises(CanvasError, cv.fill_box, 0, 0, 2, '#', '#')
        self.assertRaises(CanvasError, cv.fill_box, 0, 0, 2, 2, 0)

    def test_draw_triangle(self):
        """ module canvas: Canvas.draw_triangle()
        """
        cv = Canvas(10, 10)
        self.assertEqual(0, cv.draw_triangle(0, 0, 2, 2, 4, 0, '#'))
        self.assertRaises(CanvasError, cv.draw_triangle,
                          0, 0, 2, 2, 4, '#', '#')
        self.assertRaises(CanvasError, cv.draw_triangle,
                          0, 0, 2, 2, 4, 0, 0)

    def test_draw_thin_triangle(self):
        """ module canvas: Canvas.draw_thin_triangle()
        """
        cv = Canvas(10, 10)
        self.assertEqual(0, cv.draw_thin_triangle(0, 0, 2, 2, 4, 0))
        self.assertRaises(CanvasError, cv.draw_thin_triangle,
                          0, 0, 2, 2, 4, '#')

    def test_fill_triangle(self):
        """ module canvas: Canvas.fill_triangle()
        """
        cv = Canvas(10, 10)
        self.assertEqual(0, cv.fill_triangle(0, 0, 2, 2, 4, 0, '#'))
        self.assertRaises(CanvasError, cv.fill_triangle,
                          0, 0, 2, 2, 4, '#', '#')
        self.assertRaises(CanvasError, cv.fill_triangle,
                          0, 0, 2, 2, 4, 0, 0)

