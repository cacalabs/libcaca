require 'caca.so'

module Caca
    class Display
        attr_reader :canvas
    end
    class Event
        def Event.to_i
            const_get("TYPE")
        end
        def Event.|(i)
            i = i.to_i
            const_get("TYPE")|i
        end
        def quit?
            false
        end
        class Key
            attr_reader :ch, :utf32, :utf8
            def initialize(ch, utf32, utf8)
                @ch, @utf32, @utf8 = ch, utf32, utf8
            end
        end
        class Mouse
            attr_reader :x, :y, :button
            def initialize(x, y, button)
                @x, @y, @button = x, y, button
            end
        end
        class Resize
            attr_reader :w, :h
            def initialize(w, h)
                @w, @h = w, h
            end
        end
        class Quit
            def quit?
                true
            end
        end
    end
end
