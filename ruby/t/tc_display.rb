require 'caca'

class TC_Canvas < MiniTest::Test
    def test_create
        d = Caca::Display.new()
        refute_nil(d, 'Display creation failed')
    end
    def test_create_with_driver
        d = Caca::Display.new(Caca::Display.driver_list[0])
        refute_nil(d, 'Display creation failed')
    end
    def test_create_wrong_args
        c = Caca::Canvas.new(3, 3)
        assert_raises(RuntimeError){Caca::Display.new("plop")}
        assert_raises(RuntimeError){Caca::Display.new(c, "plop")}
        assert_raises(ArgumentError){Caca::Display.new("plop", "plop")}
        assert_raises(ArgumentError){Caca::Display.new(c, c)}
    end
    def test_create_from_canvas
        c = Caca::Canvas.new(3, 3)
        d = Caca::Display.new(c)
        refute_nil(d, 'Display creation failed')
        assert_equal(d.canvas, c, 'Wrong canvas')
    end
    def test_set_title
        c = Caca::Canvas.new(3, 3)
        d = Caca::Display.new(c)
        d.title = "Test !"
    end
    def test_set_cursor
        d = Caca::Display.new()
        d.cursor = 1
    end
end
