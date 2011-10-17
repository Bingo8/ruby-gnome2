/* -*- c-file-style: "ruby"; indent-tabs-mode: nil -*- */
/************************************************

  rbgdk.c -

  $Author: ggc $
  $Date: 2007/07/13 16:07:30 $

  Copyright (C) 2002,2003 Ruby-GNOME2 Project Team
  Copyright (C) 1998-2000 Yukihiro Matsumoto,
                          Daisuke Kanda,
                          Hiroshi Igarashi
************************************************/

#include "global.h"

VALUE mGdk;

/* We don't need them.
void        gdk_init                        (gint *argc,
                                             gchar ***argv);
gboolean    gdk_init_check                  (gint *argc,
                                             gchar ***argv);
void        gdk_parse_args                  (gint *argc,
                                             gchar ***argv);
*/

struct rbgdk_rval2gdkpoints_args {
    VALUE ary;
    long n;
    GdkPoint *result;
};

static VALUE
rbgdk_rval2gdkpoints_body(VALUE value)
{
    long i;
    struct rbgdk_rval2gdkpoints_args *args = (struct rbgdk_rval2gdkpoints_args *)value;

    for (i = 0; i < args->n; i++) {
        VALUE points = rb_ary_to_ary(RARRAY_PTR(args->ary)[i]);

        if (RARRAY_LEN(points) != 2)
            rb_raise(rb_eArgError, "point %ld should be array of size 2", i);

        args->result[i].x = NUM2INT(RARRAY_PTR(points)[0]);
        args->result[i].y = NUM2INT(RARRAY_PTR(points)[1]);
    }

    return Qnil;
}

static G_GNUC_NORETURN VALUE
rbgdk_rval2gdkpoints_rescue(VALUE value)
{
    g_free(((struct rbgdk_rval2gdkpoints_args *)value)->result);

    rb_exc_raise(rb_errinfo());
}

GdkPoint *
rbgdk_rval2gdkpoints(VALUE value, long *n)
{
    struct rbgdk_rval2gdkpoints_args args;

    args.ary = rb_ary_to_ary(value);
    args.n = RARRAY_LEN(args.ary);
    args.result = g_new(GdkPoint, args.n + 1);

    rb_rescue(rbgdk_rval2gdkpoints_body, (VALUE)&args,
              rbgdk_rval2gdkpoints_rescue, (VALUE)&args);

    *n = args.n;

    return args.result;
}

struct rbgdk_rval2gdkatoms_args {
    VALUE ary;
    long n;
    GdkAtom *result;
};

static VALUE
rbgdk_rval2gdkatoms_body(VALUE value)
{
    long i;
    struct rbgdk_rval2gdkatoms_args *args = (struct rbgdk_rval2gdkatoms_args *)value;

    for (i = 0; i < args->n; i++)
        args->result[i] = RVAL2ATOM(RARRAY_PTR(args->ary)[i]);

    return Qnil;
}

static G_GNUC_NORETURN VALUE
rbgdk_rval2gdkatoms_rescue(VALUE value)
{
    g_free(((struct rbgdk_rval2gdkatoms_args *)value)->result);

    rb_exc_raise(rb_errinfo());
}

GdkAtom *
rbgdk_rval2gdkatoms(VALUE value, long *n)
{
    struct rbgdk_rval2gdkatoms_args args;

    args.ary = rb_ary_to_ary(value);
    args.n = RARRAY_LEN(args.ary);
    args.result = g_new(GdkAtom, args.n + 1);

    rb_rescue(rbgdk_rval2gdkatoms_body, (VALUE)&args,
              rbgdk_rval2gdkatoms_rescue, (VALUE)&args);

    *n = args.n;

    return args.result;
}

struct rval2gdkpixbufglist_args {
    VALUE ary;
    long n;
    GList *result;
};

static VALUE
rbgdk_rval2gdkpixbufglist_body(VALUE value)
{
    long i;
    struct rval2gdkpixbufglist_args *args = (struct rval2gdkpixbufglist_args *)value;

    for (i = 0; i < args->n; i++)
        args->result = g_list_append(args->result, GDK_PIXBUF(RVAL2GOBJ(RARRAY_PTR(args->ary)[i])));

    return Qnil;
}

static G_GNUC_NORETURN VALUE
rbgdk_rval2gdkpixbufglist_rescue(VALUE value)
{
    g_list_free(((struct rval2gdkpixbufglist_args *)value)->result);

    rb_exc_raise(rb_errinfo());
}

GList *
rbgdk_rval2gdkpixbufglist(VALUE value)
{
    struct rval2gdkpixbufglist_args args;

    args.ary = rb_ary_to_ary(value);
    args.n = RARRAY_LEN(args.ary);
    args.result = NULL;

    rb_rescue(rbgdk_rval2gdkpixbufglist_body, (VALUE)&args,
              rbgdk_rval2gdkpixbufglist_rescue, (VALUE)&args);

    return args.result;
}

#if GTK_CHECK_VERSION(2,2,0)
static VALUE
gdk_s_get_display_arg_name(G_GNUC_UNUSED VALUE self)
{
    return CSTR2RVAL(gdk_get_display_arg_name());
}
#endif

static VALUE
gdk_s_set_locale(G_GNUC_UNUSED VALUE self)
{
    return CSTR2RVAL(gdk_set_locale());
}

static VALUE
gdk_s_set_sm_client_id(VALUE self, VALUE id)
{
    gdk_set_sm_client_id(NIL_P(id) ? NULL : RVAL2CSTR(id));
    return self;
}

#if GTK_CHECK_VERSION(2,2,0)
static VALUE
gdk_s_notify_startup_complete(int argc, VALUE *argv, VALUE self)
{
    VALUE startup_id;

    rb_scan_args(argc, argv, "01", &startup_id);

    if (NIL_P(startup_id)) {
	gdk_notify_startup_complete();
    } else {
#if GTK_CHECK_VERSION(2, 12, 0)
	gdk_notify_startup_complete_with_id(RVAL2CSTR(startup_id));
#else
	rb_raise(rb_eNotImpError,
		 "GTK+ >= 2.12 is required to use "
		 "Gdk.notify_startup_complete with startup ID.");
#endif
    }
    return self;
}
#endif

static VALUE
gdk_s_get_program_class(G_GNUC_UNUSED VALUE self)
{
    return CSTR2RVAL(gdk_get_program_class());
}

static VALUE
gdk_s_set_program_class(VALUE self, VALUE program_class)
{
    gdk_set_program_class(RVAL2CSTR(program_class));
    return self;
}

static VALUE
gdk_s_get_display(G_GNUC_UNUSED VALUE self)
{
    return CSTR2RVAL(gdk_get_display());
}

#ifdef HAVE_X11_XLIB_H 
#ifdef HAVE_XGETERRORTEXT
#include <X11/Xlib.h>
#include <errno.h>

static VALUE rb_x_error;
static VALUE rb_x_io_error;

static int
rbgdk_x_error(Display *display, XErrorEvent *error)
{
     gchar buf[64];
                                                                                
     XGetErrorText(display, error->error_code, buf, 63);

     rb_funcall((VALUE)rb_x_error, id_call, 4, INT2NUM(error->serial),
                INT2NUM(error->error_code), INT2NUM(error->request_code),
                INT2NUM(error->minor_code));
     return 0;
}

static int
rbgdk_x_io_error(Display *display)
{
    int errno_saved = errno;
    const gchar* disp;
    const gchar* error;

#if GTK_CHECK_VERSION(2,2,0)
    disp = display ? DisplayString(display) : gdk_get_display_arg_name();
#else
    disp = DisplayString(display);
#endif
    if (! disp)
        disp = "(none)";

    error = g_strerror(errno_saved);
    
    rb_funcall((VALUE)rb_x_io_error, id_call, 3, CSTR2RVAL(disp), 
               INT2NUM(errno), CSTR2RVAL(error));
    return 0;
}
#endif 
#endif 

static VALUE
gdk_s_set_x_error_handler(VALUE self)
{
#ifdef HAVE_XGETERRORTEXT
    rb_x_error = rb_block_proc();
    G_RELATIVE(self, rb_x_error);
    XSetErrorHandler(rbgdk_x_error);
#else
    rb_warn("Not supported on this environment.");
#endif
    return self;
}

static VALUE
gdk_s_set_x_io_error_handler(VALUE self)
{
#ifdef HAVE_XGETERRORTEXT
    rb_x_io_error = rb_block_proc();
    G_RELATIVE(self, rb_x_io_error);
    XSetIOErrorHandler(rbgdk_x_io_error);
#else
    rb_warn("Not supported on this environment.");
#endif
    return self;
}

static VALUE
gdk_s_flush(VALUE self)
{
    gdk_flush();
    return self;
}

static VALUE
gdk_s_screen_width(G_GNUC_UNUSED VALUE self)
{
    return INT2NUM(gdk_screen_width());
}

static VALUE
gdk_s_screen_width_mm(G_GNUC_UNUSED VALUE self)
{
    return INT2NUM(gdk_screen_width_mm());
}

static VALUE
gdk_s_screen_height(G_GNUC_UNUSED VALUE self)
{
    return INT2NUM(gdk_screen_height());
}

static VALUE
gdk_s_screen_height_mm(G_GNUC_UNUSED VALUE self)
{
    return INT2NUM(gdk_screen_height_mm());
}

static VALUE
gdk_s_pointer_grab(G_GNUC_UNUSED VALUE self, VALUE win, VALUE owner_events, VALUE event_mask, VALUE confine_to, VALUE cursor, VALUE time)
{
    return GENUM2RVAL(gdk_pointer_grab(GDK_WINDOW(RVAL2GOBJ(win)),
                     RVAL2CBOOL(owner_events),
                     RVAL2GFLAGS(event_mask, GDK_TYPE_EVENT_MASK),
                     NIL_P(confine_to)?NULL:GDK_WINDOW(RVAL2GOBJ(confine_to)),
                     NIL_P(cursor)?NULL:(GdkCursor*)RVAL2BOXED(cursor, GDK_TYPE_CURSOR),
                     NUM2INT(time)), GDK_TYPE_GRAB_STATUS);
}

static VALUE
gdk_s_pointer_ungrab(VALUE self, VALUE time)
{
    gdk_pointer_ungrab(NUM2INT(time));
    return self;
}

static VALUE
gdk_s_keyboard_grab(G_GNUC_UNUSED VALUE self, VALUE win, VALUE owner_events, VALUE time)
{
    return GENUM2RVAL(gdk_keyboard_grab(GDK_WINDOW(RVAL2GOBJ(win)), 
                                        RVAL2CBOOL(owner_events), NUM2INT(time)), 
                      GDK_TYPE_GRAB_STATUS);
}

static VALUE
gdk_s_keyboard_ungrab(VALUE self, VALUE time)
{
    gdk_keyboard_ungrab(NUM2INT(time));
    return self;
}

static VALUE
gdk_s_pointer_is_grabbed(G_GNUC_UNUSED VALUE self)
{
    return CBOOL2RVAL(gdk_pointer_is_grabbed());
}

static VALUE
gdk_s_set_double_click_time(VALUE self, VALUE msec)
{
    gdk_set_double_click_time(NUM2UINT(msec));
    return self;
}

static VALUE
gdk_s_beep(VALUE self)
{
    gdk_beep();
    return self;
}

static VALUE
gdk_s_error_trap_push(VALUE self)
{
    gdk_error_trap_push();
    return self;
}

static VALUE
gdk_s_error_trap_pop(VALUE self)
{
    gdk_error_trap_pop();
    return self;
}

static VALUE
gdk_s_windowing_x11(G_GNUC_UNUSED VALUE self)
{
#ifdef GDK_WINDOWING_X11
    return Qtrue;
#else
    return Qfalse;
#endif
}

static VALUE
gdk_s_windowing_win32(G_GNUC_UNUSED VALUE self)
{
#ifdef GDK_WINDOWING_WIN32
    return Qtrue;
#else
    return Qfalse;
#endif
}

static VALUE
gdk_s_windowing_fb(G_GNUC_UNUSED VALUE self)
{
#ifdef GDK_WINDOWING_FB
    return Qtrue;
#else
    return Qfalse;
#endif
}

static VALUE
gdk_s_windowing_quartz(G_GNUC_UNUSED VALUE self)
{
#ifdef GDK_WINDOWING_QUARTZ
    return Qtrue;
#else
    return Qfalse;
#endif
}

static VALUE
gdk_s_windowing_directfb(G_GNUC_UNUSED VALUE self)
{
#ifdef GDK_WINDOWING_DIRECTFB
    return Qtrue;
#else
    return Qfalse;
#endif
}

static VALUE
gdk_m_target(G_GNUC_UNUSED VALUE self)
{
    return CSTR2RVAL(RUBY_GTK2_TARGET);
}

void
Init_gtk_gdk(void)
{
    mGdk = rb_define_module("Gdk");

#if GTK_CHECK_VERSION(2,2,0)
    rb_define_module_function(mGdk, "display_arg_name", gdk_s_get_display_arg_name, 0);
#endif
    rb_define_module_function(mGdk, "set_locale", gdk_s_set_locale, 0);
    rb_define_module_function(mGdk, "set_sm_client_id", gdk_s_set_sm_client_id, 1);
#if GTK_CHECK_VERSION(2, 2, 0)
    rb_define_module_function(mGdk, "notify_startup_complete",
			      gdk_s_notify_startup_complete, -1);
#endif
    rb_define_module_function(mGdk, "program_class", gdk_s_get_program_class, 0);
    rb_define_module_function(mGdk, "set_program_class", gdk_s_set_program_class, 1);
    rb_define_module_function(mGdk, "display", gdk_s_get_display, 0);

    rb_define_module_function(mGdk, "set_x_error_handler", gdk_s_set_x_error_handler , 0);
    rb_define_module_function(mGdk, "set_x_io_error_handler", gdk_s_set_x_io_error_handler , 0);
    rb_define_module_function(mGdk, "screen_width", gdk_s_screen_width, 0);
    rb_define_module_function(mGdk, "screen_width_mm", gdk_s_screen_width_mm, 0);
    rb_define_module_function(mGdk, "screen_height", gdk_s_screen_height, 0);
    rb_define_module_function(mGdk, "screen_height_mm", gdk_s_screen_height_mm, 0);
    rb_define_module_function(mGdk, "beep", gdk_s_beep, 0);
    rb_define_module_function(mGdk, "flush", gdk_s_flush, 0);
    rb_define_module_function(mGdk, "set_double_click_time", gdk_s_set_double_click_time, 1);
    rb_define_module_function(mGdk, "pointer_grab", gdk_s_pointer_grab, 6);
    rb_define_module_function(mGdk, "pointer_ungrab", gdk_s_pointer_ungrab, 1);
    rb_define_module_function(mGdk, "keyboard_grab", gdk_s_keyboard_grab, 3);
    rb_define_module_function(mGdk, "keyboard_ungrab", gdk_s_keyboard_ungrab, 1);
    rb_define_module_function(mGdk, "pointer_is_grabbed?", gdk_s_pointer_is_grabbed, 0);
    rb_define_module_function(mGdk, "error_trap_push", gdk_s_error_trap_push, 0);
    rb_define_module_function(mGdk, "error_trap_pop", gdk_s_error_trap_pop, 0);
    rb_define_module_function(mGdk, "windowing_x11?", gdk_s_windowing_x11, 0);
    rb_define_module_function(mGdk, "windowing_win32?", gdk_s_windowing_win32, 0);
    rb_define_module_function(mGdk, "windowing_fb?", gdk_s_windowing_fb, 0);
    rb_define_module_function(mGdk, "windowing_quartz?", gdk_s_windowing_quartz, 0);
    rb_define_module_function(mGdk, "windowing_directfb?", gdk_s_windowing_directfb, 0);

    rb_define_module_function(mGdk, "target", gdk_m_target, 0);
    
    /* GdkGrabStatus */
    G_DEF_CLASS(GDK_TYPE_GRAB_STATUS, "GrabStatus", mGdk);
    G_DEF_CONSTANTS(mGdk, GDK_TYPE_GRAB_STATUS, "GDK_");

    G_DEF_SETTERS(mGdk);
}

