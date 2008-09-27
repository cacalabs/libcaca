require 'caca'

class TC_Frame < Test::Unit::TestCase
	def setup
		@c = Caca::Canvas.new(3, 3)
	end
	def test_create
	      f = @c.create_frame(1)
              assert(f, 'Frame creation failed')
	      @c.free_frame(1)
        end
        def test_name
	     f = @c.create_frame(1)
	     assert(@c.frame_name, 'Failed to get frame name')
	     @c.frame_name="test"
	     assert(@c.frame_name == "test", 'Failed to set frame name')
	     @c.free_frame(1)
        end
end
