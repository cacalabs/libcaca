require 'caca'

class TC_Canvas < MiniTest::Test
    def test_create
        d = Caca::Dither.new(8, 32, 32, 32, 0, 0, 0, 0)
    end
    def test_fail_create
        assert_raises(RuntimeError) {
            d = Caca::Dither.new(-1, 32, 32, 32, 0, 0, 0, 0)
        }
    end
    def test_set_palette
        d = Caca::Dither.new(8, 32, 32, 32, 0, 0, 0, 0)
        d.palette = [[0xfff, 0xfff, 0xfff, 0xfff]] * 256
    end
    def test_fail_set_palette
        assert_raises(ArgumentError) {
            d = Caca::Dither.new(8, 32, 32, 32, 0, 0, 0, 0)
            d.palette = []
        }
    end
    def test_fail_set_palette2
        assert_raises(RuntimeError) {
            d = Caca::Dither.new(8, 32, 32, 32, 0, 0, 0, 0)
            d.palette = [[0xffff, 0, 0, 0]] * 256
        }
    end
    def test_set_brightness
        d = Caca::Dither.new(8, 32, 32, 32, 0, 0, 0, 0)
        d.brightness = 0.5
    end
    def test_set_gamma
        d = Caca::Dither.new(8, 32, 32, 32, 0, 0, 0, 0)
        d.gamma = 0.5
    end
    def test_set_contrast
        d = Caca::Dither.new(8, 32, 32, 32, 0, 0, 0, 0)
        d.contrast = 0.5
    end
end

