require 'test/unit'
require 'caca'

class TC_Canvas < Test::Unit::TestCase
    def test_create
        d = Caca::Display.new
        assert_not_nil(d, 'Display creation failed')
    end
    def test_create_with_driver
        d = Caca::Display.new(Caca::Display.driver_list[0])
        assert_not_nil(d, 'Display creation failed')
    end
    def test_create_wrong_args
        assert_raise(RuntimeError){Caca::Display.new("plop")}
        driver = Caca::Display.driver_list[0]
        assert_raise(ArgumentError){Caca::Display.new(driver, driver)}
        c = Caca::Canvas.new(3, 3)
        assert_raise(ArgumentError){Caca::Display.new(c, c)}
    end
    def test_create_from_canvas
        c = Caca::Canvas.new(3, 3)
        d = Caca::Display.new(c)
        assert_not_nil(d, 'Display creation failed')
        assert_equal(d.canvas, c, 'Wrong canvas')
    end
    def test_set_title
        c = Caca::Canvas.new(3, 3)
        d = Caca::Display.new(c)
        d.title = "Test !"
    end
end
