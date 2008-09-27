require 'caca'

class TC_Canvas < Test::Unit::TestCase
    def test_list
	assert_not_nil(Caca::Font.list)
    end
    def test_load
	Caca::Font.list.each{|f|
		font = Caca::Font.new(f)
		assert_not_nil(font)
		assert_not_nil(font.width)
		assert_not_nil(font.height)
		assert_not_nil(font.blocks)
	}
    end
    def test_fail_load
	assert_raise(RuntimeError) {
		Caca::Font.new("This font should not exist")
	}
    end
end
