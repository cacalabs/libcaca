require 'test/unit'
require 'cucul'

class TC_Canvas < Test::Unit::TestCase
    def setup
        @c = Cucul::Canvas.new(3, 3)
    end
    def test_create
        c = Cucul::Canvas.new(3, 3)
        assert(c, 'Canvas creation failed')
        assert(c.width == 3 && c.height == 3, 'Wrong size for new canvas')
    end
    def test_width
    @c.width=42
        assert_equal(c.width, 42, 'Failed to set width')
    end
    def test_height
        @c.height=42
        assert_equal(c.height, 42, 'Failed to set width')
    end
    def test_size
    @c.set_size(100,100)
        assert(@c.width == 100 && @c.height == 100, 'Failed to set size')
    end
    def test_import
        @c.import_memory("foo", "")
        assert_equal(@c.export_memory("irc"), "foo\r\n", "Import/Export failed")
    end
end
