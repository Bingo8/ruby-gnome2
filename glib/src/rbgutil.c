/* -*- c-file-style: "ruby"; indent-tabs-mode: nil -*- */
/************************************************

  rbgutil.c -

  $Author: mutoh $
  $Date: 2004/03/03 17:21:50 $

  Copyright (C) 2002,2003 Masao Mutoh
************************************************/

#include "global.h"

ID rbgutil_id_module_eval;

static ID id_add_one_arg_setter;
static ID id_set_property;
static ID id_to_a;
static ID id_allocate;

VALUE
rbgutil_gerror2exception(error)
    GError *error;
{
    gchar *msg = g_locale_from_utf8(error->message, -1, NULL, NULL, NULL);
    VALUE exc = rb_exc_new2(rb_eRuntimeError, msg ? msg : error->message);

    if (msg)
        g_free(msg);
    g_error_free(error);
    return exc;
}

void
rbgutil_set_properties(self, hash)
    VALUE self, hash;
{
    int i;
    VALUE ary;
    GObject* obj;

    Check_Type(hash, T_HASH);
    ary = rb_funcall(hash, id_to_a, 0);
    obj = RVAL2GOBJ(self);

    g_object_freeze_notify(obj);
    for (i = 0; i < RARRAY(ary)->len; i++) {
      rb_funcall(self, id_set_property, 2,
                 RARRAY(RARRAY(ary)->ptr[i])->ptr[0],
                 RARRAY(RARRAY(ary)->ptr[i])->ptr[1]);
    }
    g_object_thaw_notify(obj);
}

VALUE
rbgutil_glist2ary(list)
    GList *list;
{
    VALUE ary = rb_ary_new();
    while (list) {
        rb_ary_push(ary, GOBJ2RVAL(list->data));
        list = list->next;
    }
    return ary;
}

VALUE
rbgutil_glist2ary_boxed(list, gtype)
    GList *list;
    GType gtype;
{
    VALUE ary = rb_ary_new();
    while (list) {
        rb_ary_push(ary, BOXED2RVAL(list->data, gtype));
        list = list->next;
    }
    return ary;
}


VALUE
rbgutil_gslist2ary(list)
    GSList *list;
{
    VALUE ary = rb_ary_new();
    while (list) {
        rb_ary_push(ary, GOBJ2RVAL(list->data));
        list = list->next;
    }
    return ary;
}

VALUE
rbgutil_gslist2ary_boxed(list, gtype)
    GSList *list;
    GType gtype;
{
    VALUE ary = rb_ary_new();
    while (list) {
        rb_ary_push(ary, BOXED2RVAL(list->data, gtype));
        list = list->next;
    }
    return ary;
}

VALUE
rbgutil_def_setters(klass)
    VALUE klass;
{
    return rb_funcall(mGLib, id_add_one_arg_setter, 1, klass);
}

VALUE
rbgutil_sym_g2r_func(from)
    const GValue *from;
{
    const gchar *str = g_value_get_string(from);
    return str ? ID2SYM(rb_intern(str)) : Qnil;
}

#ifndef HAVE_OBJECT_ALLOCATE
VALUE
rbgutil_generic_s_new(int argc, VALUE* argv, VALUE self)
{
    VALUE obj = rb_funcall(self, id_allocate, 0);
    rb_obj_call_init(obj, argc, argv);
    return obj;
}
#endif /* HAVE_OBJECT_ALLOCATE */

VALUE
rbgutil_generic_s_gtype(VALUE klass)
{
    return rbgobj_gtype_new(CLASS2GTYPE(klass));
}

VALUE
rbgutil_generic_gtype(VALUE self)
{
    return generic_s_gtype(CLASS_OF(self));
}


void
Init_gutil()
{
    rbgutil_id_module_eval = rb_intern("module_eval");
    id_set_property = rb_intern("set_property");
    id_to_a = rb_intern("to_a");
    id_add_one_arg_setter = rb_intern("__add_one_arg_setter");
    id_allocate = rb_intern("allocate");

    rb_eval_string(
        "module GLib\n"
        "  def self.__add_one_arg_setter(klass)\n"
        "    ary = klass.instance_methods(false)\n"
#ifdef HAVE_NODE_ATTRASGN
        "    klass.module_eval do\n"
        "      ary.each do |m|\n"
        "        if /^set_(.*)/ =~ m and not ary.include? \"#{$1}=\" and instance_method(m).arity == 1\n"
        "          alias_method \"#{$1}=\", \"set_#{$1}\"\n"
        "        end\n"
        "      end\n"
        "    end\n"
#else
        "    ary.each do |m|\n"
        "      if /^set_(.*)/ =~ m and not ary.include? \"#{$1}=\" and klass.instance_method(m).arity == 1\n"
	"        begin\n"
        "          klass.module_eval(\"def #{$1}=(val); set_#{$1}(val); val; end\\n\")\n"
        "        rescue SyntaxError\n"
        "          $stderr.print \"Couldn't create #{klass}\\##{$1}=(v).\\n\" if $DEBUG\n"
        "        end\n"
        "      end\n"
        "    end\n"
#endif
        "  end\n"
        "end\n");
}
