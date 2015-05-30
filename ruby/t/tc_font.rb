require 'caca'

class TC_Canvas < MiniTest::Test
    def test_list
        refute_nil(Caca::Font.list)
    end
    def test_load
        Caca::Font.list.each{|f|
            font = Caca::Font.new(f)
            refute_nil(font)
            refute_nil(font.width)
            refute_nil(font.height)
            refute_nil(font.blocks)
        }
    end
    def test_fail_load
        assert_raises(RuntimeError) {
            Caca::Font.new("This font should not exist")
        }
    end
end
