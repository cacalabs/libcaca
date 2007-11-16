require 'test/unit'
require 'cucul'

class TC_Canvas < Test::Unit::TestCase
    def test_list
	assert_not_nil(Cucul::Font.list)
    end
    def test_load
	Cucul::Font.list.each{|f|
		font = Cucul::Font.new(f)
		assert_not_nil(font)
		assert_not_nil(font.width)
		assert_not_nil(font.height)
		assert_not_nil(font.blocks)
	}
    end
    def test_fail_load
	assert_raise(RuntimeError) {
		Cucul::Font.new("This font should not exist")
	}
    end
end
