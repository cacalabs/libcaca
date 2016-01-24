#!/bin/sh

if brew --version 2>/dev/null 2>&1; then
    # FIXME: no ncurses formula for now; also add ruby
    brew install slang doxygen
fi

