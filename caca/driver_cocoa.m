/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2006 Olivier Gutknecht <olg@no-distance.net>
 *                2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/*
 *  This file contains the libcaca Cocoa input and output driver
 */

#include "config.h"
#include "common.h"

#if defined USE_COCOA

#include <stdio.h>
#include <stdlib.h>

#import <Carbon/Carbon.h>
#include <Cocoa/Cocoa.h>

#include "caca.h"
#include "caca_internals.h"
#include "cucul.h"
#include "cucul_internals.h"

@interface CacaView : NSView
{
    NSFont* _font;
    NSRect _r;
    int xx;
    int h, w;
    uint32_t *_attrs;
    uint32_t *_chars;
    CFMutableDictionaryRef _cache;
}
- (void)setFont:(NSFont *)aFont;
- (void)updateBuffersFromCaca:(caca_display_t *)dp;

@end

@implementation CacaView

- (void)keyDown:(NSEvent *)theEvent
{
    NSLog(@"key %@", theEvent);
}

- (void)mouseMoved:(NSEvent *)theEvent
{
    NSLog(@"mouse %@", theEvent);
}

- (void)setupNewSize
{
    int fw = _r.size.width;
    int fh = _r.size.height;
    w = [self bounds].size.width /fw;
    h = [self bounds].size.height / fh;
//    NSLog(@"W %f %f %f %d %f", [self bounds].size.width , _r.size.width, [self bounds].size.width / _r.size.width, w, [self bounds].size.width-(w*fw));
//    NSLog(@"H %f %f %f %d %f", [self bounds].size.height , _r.size.height, [self bounds].size.height / _r.size.height, h, [self bounds].size.height-(h*fh));
}

- (id)initWithFrame:(NSRect)frameRect
{
    if((self = [super initWithFrame:frameRect]) != nil) {
        [[self window] makeFirstResponder:self];
    }
    return self;
}

- (NSFont *)font
{
    return _font;
}

- (void)setFont:(NSFont *)aFont
{
    [_font release];
    _font = [aFont retain];
    _r = [_font boundingRectForFont];
    _r = NSMakeRect(0, 0, ceilf(_r.size.width), ceilf(_r.size.height));
    [self setupNewSize];
    [aFont set];
}

- (void)updateBuffersFromCaca:(caca_display_t *)dp
{
NSLog(@"update buffers");
    if(_attrs)
        free(_attrs);

    _attrs = malloc(dp->cv->width * dp->cv->height * sizeof(uint32_t) * 2);
    _chars = _attrs + dp->cv->width * dp->cv->height;
    memcpy(_attrs, dp->cv->attrs, dp->cv->width * dp->cv->height * sizeof(uint32_t));
    memcpy(_chars, dp->cv->chars, dp->cv->width * dp->cv->height * sizeof(uint32_t));

    w = dp->cv->width;
    h = dp->cv->height;

    [self setNeedsDisplay:TRUE];
}

- (void)drawRect:(NSRect)rect
{
    //if([self inLiveResize]) [self setupNewSize];
    int x, y;
    int fw = _r.size.width;
    int fh = _r.size.height;
    uint32_t *attrs = _attrs;
    uint32_t *chars = _chars;
    NSGraphicsContext* viewContext = [NSGraphicsContext currentContext];
    if(!attrs || !chars)
    {
        [[NSColor blueColor] set];
        NSRectFill(rect);
        return;
    }
    [[NSColor blackColor] set];
    NSRectFill(rect);

    for(y = 0; y < h; y++)
    {
        for(x = 0; x < w; x++)
        {
            unichar c = *chars++;
            uint8_t argb[8];
            _cucul_attr_to_argb4(*attrs++, argb);

            /* FIXME: factorise this colour stuff */
            NSRect r = NSMakeRect(x * fw, y * fh, fw, fh);
            [[NSColor colorWithDeviceRed: (float)argb[1] / 255.0
                                   green: (float)argb[2] / 255.0
                                    blue: (float)argb[3] / 255.0
                                   alpha: 1.0] setFill];
            [[NSColor colorWithDeviceRed: (float)argb[5] / 255.0
                                   green: (float)argb[6] / 255.0
                                    blue: (float)argb[7] / 255.0
                                   alpha: 1.0] setStroke];
            NSRectFill(r);
//            NSLog(@"%d %d %C %d %d %@ %@ %@", x, y, c, fg, bg, NSStringFromRect(r), _colormap[fg], _colormap[bg]);
//            [[NSString stringWithCharacters:&c length:1] drawInRect:r withAttributes:[NSDictionary dictionaryWithObject:_colormap[fg] forKey:NSForegroundColorAttributeName]];
            [[NSColor colorWithDeviceRed: (float)argb[5] / 255.0
                                   green: (float)argb[6] / 255.0
                                    blue: (float)argb[7] / 255.0
                                   alpha: 1.0] setFill];
            [[NSColor colorWithDeviceRed: (float)argb[5] / 255.0
                                   green: (float)argb[6] / 255.0
                                    blue: (float)argb[7] / 255.0
                                   alpha: 1.0] setStroke];
            [[NSString stringWithCharacters:&c length:1] drawInRect:r withAttributes:nil];
//            [[NSString stringWithCharacters:&c length:1] drawInRect:r withAttributes:[NSDictionary dictionaryWithObjectsAndKeys:
//                _colormap[fg], NSForegroundColorAttributeName, _colormap[bg], NSBackgroundColorAttributeName, nil]];
}
    }
    [NSGraphicsContext setCurrentContext:viewContext];
}
@end

struct driver_private
{
    NSWindow* window;
    id view;
    NSFont* font;
    NSAutoreleasePool* pool;
};


//--------------------------------------------------------------------------------------------
static OSStatus
AppEventHandler(EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon)
{
    OSStatus    result = eventNotHandledErr;

    switch(GetEventClass(inEvent))
    {
        case kEventClassCommand:
        {
            HICommandExtended cmd;
            verify_noerr(GetEventParameter(inEvent, kEventParamDirectObject, typeHICommand, NULL, sizeof(cmd), NULL, &cmd));

            switch(GetEventKind(inEvent))
            {
                case kEventCommandProcess:
                    switch(cmd.commandID)
                    {
                        default:
                            break;
                    }
                    break;
            }
            break;
        }

        default:
            break;
    }

    return result;
}


static OSStatus
WindowEventHandler(EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon)
{
    OSStatus err = eventNotHandledErr;

    switch(GetEventClass(inEvent))
    {
        case kEventClassCommand:
        {
            HICommandExtended cmd;
            verify_noerr(GetEventParameter(inEvent, kEventParamDirectObject, typeHICommand, NULL, sizeof(cmd), NULL, &cmd));

            switch(GetEventKind(inEvent))
            {
                case kEventCommandProcess:
                    switch(cmd.commandID)
                    {
                        // Add your own command-handling cases here
                        default:
                            break;
                    }
                    break;
            }
            break;
        }

        default:
            break;
    }

    return err;
}
static int cocoa_init_graphics(caca_display_t *dp)
{
    int i;

    dp->drv.p = malloc(sizeof(struct driver_private));
    if(dp->drv.p == NULL)
        return -1;

    Handle aHand = GetNewMBar(128);
    SetMenuBar(aHand);
    MenuHandle menu = GetMenuHandle(128);
    CreateStandardWindowMenu(0, &menu);
    InsertMenu(menu, 0);

    DrawMenuBar();
    DisposeHandle(aHand);

    NSAutoreleasePool* p = [[NSAutoreleasePool alloc] init];
//    [NSApplication sharedApplication];
    NSApplicationLoad();

    NSLog(@"Start");

    NSFont* f = [NSFont fontWithName:@"Monaco" size:10];
    NSRect r = [f boundingRectForFont];

    NSRect windowRect = NSMakeRect(0, 0, dp->cv->width * r.size.width,
                                         dp->cv->height * r.size.height);
    CacaView* v = [[[CacaView alloc] initWithFrame:windowRect] autorelease];

//    [NSApp setMainMenu:[[[NSMenu alloc] initWithTitle:@"Caca"] autorelease]];
    //[NSApp setAppleMenu:[[[NSMenu alloc] initWithTitle:@"ca"] autorelease]];
    //[NSApp setDelegate:v];

    NSWindow* w = [[NSWindow alloc]
                   initWithContentRect:windowRect
                             styleMask:NSTitledWindowMask
                                        |NSResizableWindowMask
                                        |NSClosableWindowMask
                                        |NSWindowMiniaturizeButton
                               backing:NSBackingStoreBuffered
                                 defer:NO];
    [w setContentView:v];
    [v setFont:f];
    [v setupNewSize];
    [w setFrameTopLeftPoint:NSMakePoint(650, 650)];
    [w makeKeyAndOrderFront:w];
    [w setLevel:NSFloatingWindowLevel];
    [w orderFrontRegardless];

    dp->drv.p->window = w;
    dp->drv.p->view = v;
    dp->drv.p->font = f;
    dp->drv.p->pool = p;

//    NSLog(@"update4 %d %d", w, h);
//    _cucul_set_size(dp->cv, windowRect.size.width, windowRect.size.height);
    OSStatus err;
    static const EventTypeSpec kAppEvents[] =
    {
        { kEventClassCommand, kEventCommandProcess }
    };
    InstallApplicationEventHandler(NewEventHandlerUPP(AppEventHandler),
                                   GetEventTypeCount(kAppEvents), kAppEvents,
                                   0, NULL);
    NSLog(@"a");
    /* Should not be run here, should it? */
    RunApplicationEventLoop();
    NSLog(@"ab");
    return 0;
}


static void cocoa_display(caca_display_t *dp)
{
//NSLog(@"display1");
    [dp->drv.p->view updateBuffersFromCaca:dp];
#if 1
    [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate date]];
#else
    [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[[NSCalendarDate date]
    dateByAddingYears:0 months:(int)0 days:(int)0 hours:(int)0 minutes:(int)0 seconds:(int)5]];
#endif
//NSLog(@"display2");
}

static void cocoa_handle_resize(caca_display_t *dp)
{
    dp->resize.w = dp->cv->width;
    dp->resize.h = dp->cv->height;
}

static int cocoa_get_event(caca_display_t *dp, struct caca_event *ev)
{
    [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode
                             beforeDate:[NSDate date]];
    ev->type = CACA_EVENT_NONE;
    return 0;
}

static int cocoa_set_display_title(caca_display_t *dp, char const *title)
{
    [dp->drv.p->window setTitle:[NSString stringWithUTF8String:title]];
    return 0;
}

static unsigned int cocoa_get_display_width(caca_display_t *dp)
{
    return [dp->drv.p->window frame].size.width;
}

static unsigned int cocoa_get_display_height(caca_display_t *dp)
{
    return [dp->drv.p->window frame].size.height;
}

static void cocoa_set_mouse(caca_display_t *dp, int flag)
{
    if(flag)
        [[NSCursor arrowCursor] set];
    else {
        [[NSCursor disappearingItemCursor] set];
    }
}

static int cocoa_end_graphics(caca_display_t *dp)
{
    [dp->drv.p->window close];
    [dp->drv.p->window release];
    [dp->drv.p->pool release];
    [dp->drv.p->font release];
    free(dp->drv.p);
    return 0;
}

/*
 * Driver initialisation
 */

int cocoa_install(caca_display_t *dp)
{
    dp->drv.driver = CACA_DRIVER_RAW;

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

