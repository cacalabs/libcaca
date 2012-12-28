/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2006 Colin Delacroix <colin@zoy.org>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

/*
 *  This file contains the libcaca Cocoa input and output driver
 */

#include "config.h"

#if defined USE_COCOA

#import <Cocoa/Cocoa.h>

#include "caca.h"
#include "caca.h"
#include "caca_internals.h"

//#define COCOA_DEBUG

// many ways to draw the chars :
// - NSString and drawInRect:withAttributes: or drawWithRect:options:attributes:
// - NSAttributedString and drawInRect: or drawWithRect:options:
// - NSTextLayout and co.
// - Quartz 2D
// - ATSUI (more accessible from carbon)
// 2 firsts are high level cocoa, 3rd is low-level cocoa, other are untested
// also see http://www.cocoabuilder.com/archive/message/cocoa/2004/11/18/121928
// update: actually high-level is faster, so keep it like that
//#define USE_LOWLEVEL_COCOA 1

// build a complete color table cache for the view
#define PRECACHE_WHOLE_COLOR_TABLE 1

//#define USE_RGB12_FGBG 1

//#define USE_GLOBAL_AUTORELEASE_POOL 1

#ifdef COCOA_DEBUG
#define debug_log NSLog
#else
#define debug_log(...)
#endif

#define NCOLORS 0x1000

static BOOL s_quit = NO;
static BOOL s_quitting = NO;

@interface CacaView : NSView
{
    //NSFont* _font;
    NSRect _font_rect;
    int _h, _w;
    uint32_t* _attrs;
    uint32_t* _chars;
    NSRect*   _bkg_rects;
    NSColor** _bkg_colors;
#ifdef PRECACHE_WHOLE_COLOR_TABLE
    NSColor* _colorCache[NCOLORS];
#else
    NSMutableDictionary* _colorCache;
#endif
    NSMutableDictionary* _attrDict;
    NSMutableDictionary* _attrDictUnderline; // lame optim
#ifdef USE_LOWLEVEL_COCOA
    NSTextStorage* _textStorage;
    NSLayoutManager* _layoutManager;
    NSTextContainer* _textContainer;
#endif
}

- (void)setFont:(NSFont *)aFont;
- (void)updateBuffersFromCaca:(caca_display_t *)dp;
@end

@interface NSColor(Caca)
+ (NSColor *)colorFromRgb12:(uint16_t) ui_rgb12;
@end

@implementation CacaView
- (id)initWithFrame:(NSRect)frameRect
{
    self = [super initWithFrame:frameRect];
    if(!self)
        return nil;

    [[self window] makeFirstResponder:self];

#ifdef PRECACHE_WHOLE_COLOR_TABLE
    int i;
    for(i = 0; i < NCOLORS; i++)
        _colorCache[i] = [[NSColor colorFromRgb12:i] retain];
#else
    _colorCache = [[NSMutableDictionary alloc] initWithCapacity:NCOLORS];
#endif
    _attrDict = [[NSMutableDictionary alloc] initWithCapacity:3];
    _attrDictUnderline = [[NSMutableDictionary alloc] initWithCapacity:3];
    [_attrDictUnderline setObject:[NSNumber numberWithInt:NSUnderlineStyleSingle]
                        forKey:NSUnderlineStyleAttributeName];
#ifdef USE_LOWLEVEL_COCOA
    _textStorage = [[NSTextStorage alloc] init];
    _layoutManager = [[NSLayoutManager alloc] init];
    _textContainer = [[NSTextContainer alloc] init];
    [_textContainer setLineFragmentPadding:0.0];
    [_layoutManager addTextContainer:_textContainer];
    [_textStorage addLayoutManager:_layoutManager];
#endif

    return self;
}

- (void)dealloc
{
    //[_font release];
#ifdef PRECACHE_WHOLE_COLOR_TABLE
    short i;
    for(i = 0; i < NCOLORS; i++)
        [_colorCache[i] release];
#else
    [_colorCache release];
#endif
    [_attrDict release];
    [_attrDictUnderline release];
#ifdef USE_LOWLEVEL_COCOA
    [_textStorage release];
    [_layoutManager release];
    [_textContainer release];
#endif
    if(_attrs)
        free(_attrs);
    if(_bkg_rects)
        free(_bkg_rects);
    if(_bkg_colors)
        free(_bkg_colors);

    [super dealloc];
}

// to accelerate the window drawing speed
- (BOOL)isOpaque
{
    return YES;
}

- (BOOL)isFlipped
{
    return YES;
}

- (void)setupNewSize
{
    float fw = _font_rect.size.width;
    float fh = _font_rect.size.height;
    _w = ceilf([self bounds].size.width / fw);
    _h = ceilf([self bounds].size.height / fh);
    debug_log(@"fw=%f selfw=%f %u %f", fw, [self bounds].size.width,
              _w, [self bounds].size.width-(_w*fw));
    debug_log(@"fh=%f selfh=%f %u %f", fh, [self bounds].size.height,
              _h, [self bounds].size.height-(_h*fh));
}

- (void)keyDown:(NSEvent *)theEvent
{
    NSLog(@"key %@", theEvent);
}

- (void)mouseMoved:(NSEvent *)theEvent
{
    NSLog(@"mouse %@", theEvent);
}

- (void)setFont:(NSFont *)aFont
{
    //[_font release];
    //_font = [aFont retain];
    _font_rect = [aFont boundingRectForFont];
    _font_rect = NSMakeRect(0, 0, ceilf(_font_rect.size.width), ceilf(_font_rect.size.height));
    [self setupNewSize];
    [_attrDict setObject:aFont forKey:NSFontAttributeName];
    [_attrDictUnderline setObject:aFont forKey:NSFontAttributeName];
    [aFont set];
}

- (void)resizeIfNeeded:(caca_display_t *)dp
{
    if(_w != caca_get_canvas_width(dp->cv)
        || _h != caca_get_canvas_height(dp->cv)
        || !_attrs || !_bkg_rects || !_bkg_colors)
    {
        debug_log(@"%s resize to %ux%u", _cmd, _w, _h);

        _w = caca_get_canvas_width(dp->cv);
        _h = caca_get_canvas_height(dp->cv);

        if(_attrs)
            free(_attrs);
        _attrs = malloc(_w * _h * sizeof(uint32_t) * 2);

        if(_bkg_rects)
            free(_bkg_rects);
        _bkg_rects = malloc(_w * _h * sizeof(NSRect));

        if(_bkg_colors)
            free(_bkg_colors);
        _bkg_colors = malloc(_w * _h * sizeof(NSColor*));

      //  [[self window] setContentSize: NSMakeSize(caca_get_canvas_width(dp->cv) * _font_rect.size.width,
     //                                             caca_get_canvas_height(dp->cv) * _font_rect.size.height)];
          [[self window] setContentSize: NSMakeSize(caca_get_canvas_width(dp->cv) * 8,
                                                    caca_get_canvas_height(dp->cv) * 13)];

    }
}

- (void)updateBuffersFromCaca:(caca_display_t *)dp
{
    [self resizeIfNeeded:dp];

    if(_attrs)
    {
        _chars = _attrs + _w * _h;
        memcpy(_attrs, caca_get_canvas_attrs(dp->cv),
               _w * _h * sizeof(uint32_t));
        memcpy(_chars, caca_get_canvas_chars(dp->cv),
               _w * _h * sizeof(uint32_t));

        [self setNeedsDisplay:TRUE];
    }
}

- (void)drawRect:(NSRect)rect
{
    //if([self inLiveResize]) [self setupNewSize];

    if(!_attrs || !_chars)
    {
        [[NSColor blueColor] set];
        NSRectFill(rect);
        return;
    }

    int x, y;
    float fw = 8;//_font_rect.size.width;
    float fh = 13;//_font_rect.size.height;
    uint32_t* attrs;
    uint32_t* chars = _chars;

    /* first take care of the background */
    [[NSColor blackColor] set];
    NSRectFill(rect);

    int arrayLength = 0;
    for(y = 0; y < _h; y++)
    {
        int yoff = y * fh;
        for(x = 0; x < _w; x++)
        {
            NSRect r = NSMakeRect(x * fw, yoff, fw, fh);
            if(NSIntersectsRect(r, rect))
            {
                attrs = _attrs + x + y * _w;
                NSColor* color = nil;
#if USE_RGB12_FGBG
                uint16_t bg = caca_attr_to_rgb12_bg(*attrs);
                if(bg)
                {
#   ifdef PRECACHE_WHOLE_COLOR_TABLE
                    color = _colorCache[bg];
#   else
                    NSNumber* numberBg = [NSNumber numberWithInt:bg];
                    color = [_colorCache objectForKey:numberBg];
                    if(!color)
                    {
                        color = [NSColor colorFromRgb12:bg];
                        if(color)
                            [_colorCache setObject:color forKey:numberBg];
                    }
#   endif
                }
#else
                uint8_t argb[8];
                caca_attr_to_argb64(*attrs, argb);
                color =  [NSColor colorWithCalibratedRed:((float)argb[1]) / 15.0
                                  green:((float)argb[2]) / 15.0
                                  blue:((float)argb[3]) / 15.0
                                  alpha:1.0];
#endif
                if(color)
                {
                    _bkg_colors[arrayLength] = color;
                    _bkg_rects[arrayLength++] = r;
                }
            }
        }
    }
    NSRectFillListWithColors(_bkg_rects, _bkg_colors, arrayLength);

    /* Then print the foreground characters */
    for(y = 0; y < _h; y++)
    {
        int yoff = y * fh;
        for(x = 0; x < _w; x++, chars++)
        {
            attrs = _attrs + x + y * _w;

            /* Skip spaces */
            if(*chars <= 0x00000020)
                continue;

            if(*chars == CACA_MAGIC_FULLWIDTH)
                continue;

            /* Plain ASCII, no problem. */
            // TODO: test me with wide chars
            //if(*chars > 0x00000020 && *chars < 0x00000080)
            {
                NSRect r = NSMakeRect(x * fw + 1, yoff, fw - 1, fh);
                if(NSIntersectsRect(r, rect))
                {
                    NSColor* color = nil;
#if USE_RGB12_FGBG
                    uint16_t fg = caca_attr_to_rgb12_fg(*attrs);
#   ifdef PRECACHE_WHOLE_COLOR_TABLE
                    color = _colorCache[fg];
#   else // PRECACHE_WHOLE_COLOR_TABLE
                    NSNumber* numberFg = [NSNumber numberWithInt:fg];
                    color = [_colorCache objectForKey:numberFg];
                    if(!color)
                    {
                        color = [NSColor colorFromRgb12:fg];
                        if(color)
                            [_colorCache setObject:color forKey:numberFg];
                    }
#   endif // PRECACHE_WHOLE_COLOR_TABLE
#else // USE_RGB12_FGBG
                    uint8_t argb[8];
                    caca_attr_to_argb64(*attrs, argb);
                    debug_log(@"x,y=[%d,%d] r,g,b back=[%u %u %u] front=[%u %u %u]",
                              x, y, argb[1], argb[2], argb[3], argb[5], argb[6], argb[7]);
                    color =  [NSColor colorWithCalibratedRed:((float)argb[5]) / 15.0
                                      green:((float)argb[6]) / 15.0
                                      blue:((float)argb[7]) / 15.0
                                      alpha:1.0];
#endif // USE_RGB12_FGBG

                    if(color)
                    {
                        NSMutableDictionary* attrDict = (*attrs & CACA_UNDERLINE) ?
                                                        _attrDictUnderline : _attrDict;
                        [attrDict setObject:color forKey:NSForegroundColorAttributeName];

                        unichar ch = *chars;
                        NSString* str = [[NSString alloc] initWithCharacters:&ch length:1];

#ifdef USE_LOWLEVEL_COCOA
                        [[_textStorage mutableString] setString:str];
                        [_textStorage setAttributes:attrDict range:NSMakeRange(0, 1)];
                        [_layoutManager drawGlyphsForGlyphRange:NSMakeRange(0, 1) atPoint:r.origin];
#else
                        [str drawInRect:r withAttributes:attrDict];
#endif
                        [str release];
                    }
                }
                continue;
            }
        }
    }
}

@end

struct driver_private
{
    NSWindow* window;
    CacaView* view;
#ifdef USE_GLOBAL_AUTORELEASE_POOL
    NSAutoreleasePool* pool;
#endif
};

//============================================================================
// NSApplication(Caca)
//============================================================================

@implementation NSApplication(Caca)
- (void)setRunning
{
    _running = 1;
}
@end

//============================================================================
// NSColor(Caca)
//============================================================================

@implementation NSColor(Caca)
+ (NSColor *)colorFromRgb12:(uint16_t)ui_rgb12
{
    float red   = ((float)((ui_rgb12 & 0x0f00) >> 3)) / 15.0,
          green = ((float)((ui_rgb12 & 0x00f0) >> 2)) / 15.0,
          blue  = ((float)( ui_rgb12 & 0x000f)      ) / 15.0;
    return [NSColor colorWithDeviceRed:red green:green
                    blue:blue alpha:1.0];
}
@end

//============================================================================
// CacaWindowDelegate
//============================================================================

@interface CacaWindowDelegate : NSObject
@end

@implementation CacaWindowDelegate
- (BOOL)windowShouldClose:(id)sender
{
    debug_log(@"%s", _cmd);
    [NSApp terminate:self];
    return NO;
}
@end

//============================================================================
// CacaAppDelegate
//============================================================================

@interface CacaAppDelegate : NSObject
@end

@implementation CacaAppDelegate : NSObject
- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
    s_quit = YES;
    return NSTerminateCancel;
}
@end

/* setAppleMenu disappeared from the headers in 10.4 */
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
@interface NSApplication(NSAppleMenu)
- (void)setAppleMenu:(NSMenu *)menu;
@end
#endif

//============================================================================
// utility methods
//============================================================================

static NSString* get_application_name()
{
    NSString* appName = [[NSBundle mainBundle] objectForInfoDictionaryKey:
                                                   @"CFBundleName"];
    if(![appName length])
        appName = [[NSProcessInfo processInfo] processName];

    return appName;
}

static void create_application_menus()
{
    /* Create the main menu bar */
    [NSApp setMainMenu:[[NSMenu alloc] init]];

    /* Create the application menu */
    NSString* appName = get_application_name();
    NSMenu* appleMenu = [[NSMenu alloc] initWithTitle:@""];

    /* Add menu items */
    NSString* title = [@"About " stringByAppendingString:appName];
    [appleMenu addItemWithTitle:title
               action:@selector(orderFrontStandardAboutPanel:)
               keyEquivalent:@""];
    [appleMenu addItem:[NSMenuItem separatorItem]];

    title = [@"Hide " stringByAppendingString:appName];
    [appleMenu addItemWithTitle:title action:@selector(hide:)
               keyEquivalent:@"h"];

    id<NSMenuItem> menuItem = [appleMenu addItemWithTitle:@"Hide Others"
                                         action:@selector(hideOtherApplications:)
                                         keyEquivalent:@"h"];
    [menuItem setKeyEquivalentModifierMask:(NSAlternateKeyMask|NSCommandKeyMask)];

    [appleMenu addItemWithTitle:@"Show All"
               action:@selector(unhideAllApplications:)
               keyEquivalent:@""];
    [appleMenu addItem:[NSMenuItem separatorItem]];

    title = [@"Quit " stringByAppendingString:appName];
    [appleMenu addItemWithTitle:title action:@selector(terminate:)
               keyEquivalent:@"q"];

    /* Put menu into the menubar */
    menuItem = [[NSMenuItem alloc] initWithTitle:@"" action:nil keyEquivalent:@""];
    [menuItem setSubmenu:appleMenu];
    [[NSApp mainMenu] addItem:menuItem];
    [menuItem release];

    /* Tell the application object that this is now the application menu */
    [NSApp setAppleMenu:appleMenu];
    [appleMenu release];

    /* Create the window menu */
    NSMenu* windowMenu = [[NSMenu alloc] initWithTitle:@"Window"];

    /* "Minimize" item */
    menuItem = [[NSMenuItem alloc] initWithTitle:@"Minimize"
                                   action:@selector(performMiniaturize:)
                                   keyEquivalent:@"m"];
    [windowMenu addItem:menuItem];
    [menuItem release];

    /* Put menu into the menubar */
    menuItem = [[NSMenuItem alloc] initWithTitle:@"Window" action:nil keyEquivalent:@""];
    [menuItem setSubmenu:windowMenu];
    [[NSApp mainMenu] addItem:menuItem];
    [menuItem release];

    /* Tell the application object that this is now the window menu */
    [NSApp setWindowsMenu:windowMenu];
    [windowMenu release];
}

static void register_cocoa_app(caca_display_t *dp)
{
    ProcessSerialNumber psn;
    if(!GetCurrentProcess(&psn))
    {
        TransformProcessType(&psn, kProcessTransformToForegroundApplication);
        SetFrontProcess(&psn);
    }

    if(NSApp == nil)
    {
        [NSApplication sharedApplication];

        if(![NSApp mainMenu])
            create_application_menus();

        [NSApp finishLaunching];
    }

    if ([NSApp delegate] == nil)
        [NSApp setDelegate:[[CacaAppDelegate alloc] init]];

    [NSApp setRunning];
}

static __inline__ void convert_NSRect(NSRect *r)
{
    float mb_height = 38.0; // [[NSApp mainMenu] menuBarHeight] is 0 - wtf ?
    /*debug_log(@"%@ %f %f %d %d %d", [NSApp mainMenu],
             [[NSApp mainMenu] menuBarHeight], mb_height,
             (int)CGDisplayPixelsHigh(kCGDirectMainDisplay),
             (int)r->origin.y, (int)r->size.height);*/
    r->origin.y = CGDisplayPixelsHigh(kCGDirectMainDisplay) - mb_height
                  - r->origin.y - r->size.height;
}

static void create_first_window(caca_display_t *dp)
{
    NSFont* font = [NSFont fontWithName:@"Monaco" size:10];
    NSRect fontRect = [font boundingRectForFont];
    fontRect = NSMakeRect(0, 0, ceilf(fontRect.size.width), ceilf(fontRect.size.height));
    NSRect windowRect = NSMakeRect(20, 20, caca_get_canvas_width(dp->cv) * fontRect.size.width,
                                           caca_get_canvas_height(dp->cv) * fontRect.size.height);
    convert_NSRect(&windowRect);

    CacaView* view = [[CacaView alloc] initWithFrame:windowRect];
    NSWindow* win = [[NSWindow alloc] initWithContentRect:windowRect
                                      styleMask:  NSTitledWindowMask
                                              //| NSResizableWindowMask
                                                | NSClosableWindowMask
                                                | NSWindowMiniaturizeButton
                                      backing:NSBackingStoreBuffered
                                      defer:NO];

    NSString* appName = get_application_name();
    if(appName)
        [win setTitle: appName];
    [win setDelegate:[CacaWindowDelegate new]];
    [win setContentView:view];
    [view setFont:font];
    [win makeKeyAndOrderFront:nil];

    dp->drv.p->window = win;
    dp->drv.p->view = view;
}

static int get_caca_keycode(NSEvent* event)
{
    int caca_keycode = 0;
    /*
    unsigned short mac_keycode = [event keyCode];
    debug_log(@"keycode %u (%x)", mac_keycode, mac_keycode);
    switch(mac_keycode)
    {
    }
    */
    if(/*!caca_keycode &&*/ ([event modifierFlags] & NSControlKeyMask))
    {
        NSString *chars = [event charactersIgnoringModifiers];
        unichar ch = [chars characterAtIndex: 0];
        // CACA_KEY_CTRL_A -> CACA_KEY_CTRL_Z
        if(ch >= 'a' && ch <= 'z')
            caca_keycode = CACA_KEY_CTRL_A + ch - 'a';
    }

    if(!caca_keycode)
    {
        NSString *chars = [event characters];
        unichar ch = 0;
        if([chars length])
            ch = [chars characterAtIndex: 0];
        switch(ch)
        {
            case NSUpArrowFunctionKey:
                caca_keycode = CACA_KEY_UP;
                break;
            case NSDownArrowFunctionKey:
                caca_keycode = CACA_KEY_DOWN;
                break;
            case NSLeftArrowFunctionKey:
                caca_keycode = CACA_KEY_LEFT;
                break;
            case NSRightArrowFunctionKey:
                caca_keycode = CACA_KEY_RIGHT;
                break;
            case 27:
                caca_keycode = CACA_KEY_ESCAPE;
                break;
            case NSDeleteCharacter:
                caca_keycode = CACA_KEY_DELETE;
                break;
            case NSBackspaceCharacter:
                caca_keycode = CACA_KEY_BACKSPACE;
                break;
            case NSTabCharacter:
                caca_keycode = CACA_KEY_TAB;
                break;
            case NSNewlineCharacter:
            case NSCarriageReturnCharacter:
                caca_keycode = CACA_KEY_RETURN;
                break;
            case NSPageUpFunctionKey:
                caca_keycode = CACA_KEY_PAGEUP;
                break;
            case NSPageDownFunctionKey:
                caca_keycode = CACA_KEY_PAGEDOWN;
                break;
            case NSF1FunctionKey:
                caca_keycode = CACA_KEY_F1;
                break;
            case NSF2FunctionKey:
                caca_keycode = CACA_KEY_F2;
                break;
            case NSF3FunctionKey:
                caca_keycode = CACA_KEY_F3;
                break;
            case NSF4FunctionKey:
                caca_keycode = CACA_KEY_F4;
                break;
            case NSF5FunctionKey:
                caca_keycode = CACA_KEY_F5;
                break;
            case NSF6FunctionKey:
                caca_keycode = CACA_KEY_F6;
                break;
            case NSF7FunctionKey:
                caca_keycode = CACA_KEY_F7;
                break;
            case NSF8FunctionKey:
                caca_keycode = CACA_KEY_F8;
                break;
            case NSF9FunctionKey:
                caca_keycode = CACA_KEY_F9;
                break;
            case NSF10FunctionKey:
                caca_keycode = CACA_KEY_F10;
                break;
            case NSF11FunctionKey:
                caca_keycode = CACA_KEY_F11;
                break;
            case NSF12FunctionKey:
                caca_keycode = CACA_KEY_F12;
                break;
            case NSF13FunctionKey:
                caca_keycode = CACA_KEY_F13;
                break;
            case NSF14FunctionKey:
                caca_keycode = CACA_KEY_F14;
                break;
            case NSF15FunctionKey:
                caca_keycode = CACA_KEY_F15;
                break;
            case NSPauseFunctionKey:
                caca_keycode = CACA_KEY_PAUSE;
                break;
            case NSInsertFunctionKey:
                debug_log(@"insert key");
                caca_keycode = CACA_KEY_INSERT;
                break;
            case NSHomeFunctionKey:
                caca_keycode = CACA_KEY_HOME;
                break;
            case NSEndFunctionKey:
                caca_keycode = CACA_KEY_END;
                break;
        }
    }

    return caca_keycode;
}

static BOOL handle_key_event(caca_privevent_t *ev, NSEvent* event)
{
    if(!ev || !event)
        return NO;

    BOOL eventHandled = NO;

    if([event modifierFlags] & NSCommandKeyMask)
    {
        // let the system handle the Apple-commands for now
        return NO;
    }

    switch ([event type]) {
        case NSKeyDown:
            /* test [event isARepeat] ? */
            ev->type = CACA_EVENT_KEY_PRESS;
            break;
        case NSKeyUp:
            ev->type = CACA_EVENT_KEY_RELEASE;
            break;
        default:
            ;
    }

    int caca_keycode = get_caca_keycode(event);
    if(caca_keycode)
    {
        ev->data.key.ch = caca_keycode;
        eventHandled = YES;
    }
    else
    {
        NSString *chars = [event characters];
        unichar mac_keycode = 0;
        if([chars length])
            mac_keycode = [chars characterAtIndex: 0];
        if(mac_keycode)
        {
            ev->data.key.ch = mac_keycode;
            ev->data.key.utf32 = (uint32_t)mac_keycode;
            ev->data.key.utf8[0] = mac_keycode & 0x00ff; // FIXME: endianness
            ev->data.key.utf8[1] = mac_keycode & 0xff00;

            eventHandled = YES;
        }
    }

    return eventHandled;
}

// TODO: handle CACA_EVENT_RESIZE
static BOOL handle_mouse_event(caca_display_t *dp, caca_privevent_t *ev,
                               NSEvent* event)
{
    if(!ev || !event)
        return NO;

    switch ([event type]) {
        case NSLeftMouseDown:
            ev->type = CACA_EVENT_MOUSE_PRESS;
            ev->data.mouse.button = 1;
            break;
        case NSLeftMouseUp:
            ev->type = CACA_EVENT_MOUSE_RELEASE;
            ev->data.mouse.button = 1;
            break;
        case NSRightMouseDown:
            ev->type = CACA_EVENT_MOUSE_PRESS;
            ev->data.mouse.button = 2;
            break;
        case NSRightMouseUp:
            ev->type = CACA_EVENT_MOUSE_RELEASE;
            ev->data.mouse.button = 2;
            break;
        case NSMouseMoved:
        {
            NSPoint mouseLoc = [NSEvent mouseLocation];
            int mouse_x = round(mouseLoc.x);
            int mouse_y = round(mouseLoc.y);
            if(dp->mouse.x == mouse_x && dp->mouse.y == mouse_y)
                break;

            dp->mouse.x = mouse_x;
            dp->mouse.y = mouse_y;

            ev->type = CACA_EVENT_MOUSE_MOTION;
            ev->data.mouse.x = dp->mouse.x;
            ev->data.mouse.y = dp->mouse.y;
            break;
        }
        default:
            ;
    }

    return YES;
}

//============================================================================
// caca driver methods
//============================================================================

static int cocoa_init_graphics(caca_display_t *dp)
{
    int width = caca_get_canvas_width(dp->cv);
    int height = caca_get_canvas_height(dp->cv);

    debug_log(@"%s dp->cv: %ux%u", __PRETTY_FUNCTION__, width, height);

    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

    dp->drv.p = malloc(sizeof(struct driver_private));
    if(dp->drv.p == NULL)
        return -1;

    dp->resize.allow = 1;
    caca_set_canvas_size(dp->cv, width ? width : 80, height ? height : 32);
    dp->resize.allow = 0;

    // first create a full cocoa app if the host has no bundle
    if(![[NSBundle mainBundle] bundleIdentifier])
        register_cocoa_app(dp);
    create_first_window(dp);

#ifdef USE_GLOBAL_AUTORELEASE_POOL
    dp->drv.p->pool = pool;
#else
    [pool release];
#endif

    return 0;
}

static int cocoa_end_graphics(caca_display_t *dp)
{
    debug_log(@"%s dp->cv: %ux%u", __PRETTY_FUNCTION__,
              caca_get_canvas_width(dp->cv), caca_get_canvas_height(dp->cv));

    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    [dp->drv.p->window close];
    CacaWindowDelegate* delegate = [dp->drv.p->window delegate];
    [dp->drv.p->window setDelegate:nil];
    [delegate release];
    // don't release the window yourself
    //[dp->drv.p->window release];
#ifdef USE_GLOBAL_AUTORELEASE_POOL
    [dp->drv.p->pool release];
#endif
    free(dp->drv.p);
    debug_log(@"%s end", __PRETTY_FUNCTION__);
    [pool release];

    return 0;
}

static void cocoa_display(caca_display_t *dp)
{
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    [dp->drv.p->view updateBuffersFromCaca:dp];
    [pool release];
}

static int cocoa_get_event(caca_display_t *dp, caca_privevent_t *ev)
{
    if(s_quit)
    {
        if(s_quitting)
        {
            // host app isn't handling the quit event properly, aborting
            debug_log(@"duplicate quit event, aborting.");
            abort();
        }
        debug_log(@"posting quit event.");
        ev->type = CACA_EVENT_QUIT;
        s_quitting = YES;
        return 1;
    }

    BOOL eventHandled = NO, forceRedispatch = NO;
    ev->type = CACA_EVENT_NONE;
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

    if([NSApp isRunning])
    {
        NSEvent *event = [NSApp nextEventMatchingMask:NSAnyEventMask
                                untilDate:[NSDate distantPast]
                                inMode:NSDefaultRunLoopMode
                                dequeue:YES];
        if(event)
        {
            switch([event type])
            {
                case NSKeyDown:
                case NSKeyUp:
                    eventHandled = handle_key_event(ev, event);
                    break;

                case NSFlagsChanged:
                    break;

                case NSLeftMouseDown:
                case NSLeftMouseUp:
                case NSRightMouseDown:
                case NSRightMouseUp:
                case NSMouseMoved:
                    if([NSApp isActive])
                    {
                        eventHandled = handle_mouse_event(dp, ev, event);
                        forceRedispatch = YES;
                    }
                    else
                    {
                        [NSApp sendEvent:event];
                        eventHandled = YES;
                    }
                    break;

                default:
                    ;
            }

            if(!eventHandled || forceRedispatch)
                [NSApp sendEvent:event];
        }
    }
    [pool release];

    if(eventHandled)
        return 1;

    return 0;
}

static void cocoa_handle_resize(caca_display_t *dp)
{
    debug_log(@"%s", __PRETTY_FUNCTION__);
    dp->resize.w = caca_get_canvas_width(dp->cv);
    dp->resize.h = caca_get_canvas_height(dp->cv);
}

static int cocoa_set_display_title(caca_display_t *dp, char const *title)
{
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    [dp->drv.p->window setTitle:[NSString stringWithUTF8String:title]];
    [pool release];
    return 0;
}

static int cocoa_get_display_width(caca_display_t const *dp)
{
    return [dp->drv.p->window frame].size.width;
}

static int cocoa_get_display_height(caca_display_t const *dp)
{
    return [dp->drv.p->window frame].size.height;
}

static void cocoa_set_mouse(caca_display_t *dp, int flag)
{
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    if(flag)
        [[NSCursor arrowCursor] set];
    else {
        [[NSCursor disappearingItemCursor] set];
    }
    [pool release];
}

/*
 * Driver initialisation
 */

int cocoa_install(caca_display_t *dp)
{
    dp->drv.id = CACA_DRIVER_COCOA;
    dp->drv.driver = "cocoa";

    dp->drv.init_graphics = cocoa_init_graphics;
    dp->drv.end_graphics = cocoa_end_graphics;
    dp->drv.set_display_title = cocoa_set_display_title;
    dp->drv.get_display_width = cocoa_get_display_width;
    dp->drv.get_display_height = cocoa_get_display_height;
    dp->drv.display = cocoa_display;
    dp->drv.handle_resize = cocoa_handle_resize;
    dp->drv.get_event = cocoa_get_event;
    dp->drv.set_mouse = cocoa_set_mouse;

    return 0;
}

#endif /* USE_COCOA */
