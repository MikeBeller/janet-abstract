#include "abstract.h"

/* Types */

typedef struct {
    JanetTable *t;
} abstract_t;

/* Utility */
JanetFunction *get_dunder_function(abstract_t *abs, char *meth_name) {
    JanetTable *proto = abs->t->proto;
    if (proto == NULL) {
        return NULL;
    }
    Janet key = janet_ckeywordv(meth_name);
    Janet fun = janet_table_get(proto, key);
    if (janet_checktype(fun, JANET_NIL) || !janet_checktype(fun, JANET_FUNCTION)) {
        return NULL;
    }
    return janet_unwrap_function(fun);
}

/* Deinitialising */

static int abstract_gc(void *p, size_t size) {
    (void) size;
    (void) p;
    return 0;
}

/* Marking */

static int abstract_mark(void *p, size_t size) {
    (void) size;
    abstract_t *abs = (abstract_t *)p;
    janet_mark(janet_wrap_table(abs->t));
    return 0;
}

/* Accessing */

static int abstract_get(void *p, Janet key, Janet *out) {
    abstract_t *abs = (abstract_t *)p;
    JanetFunction *fun = get_dunder_function(abs, "__get");
    if (fun != NULL) {
        Janet argv[] = {janet_wrap_table(abs->t), key};
        Janet r = janet_call(fun, 2, argv);
        if (!janet_checktype(r, JANET_NIL)) {
            *out = r;
            return 1;
        }
    } else {
        JanetKV *bucket = janet_table_find(abs->t, key);
        if (NULL != bucket && !janet_checktype(bucket->key, JANET_NIL)) {
            *out = bucket->value;
            return 1;
        }
    }
    return 0;
}

/* Inserting */

static void abstract_put(void *p, Janet key, Janet val) {
    abstract_t *abs = (abstract_t *)p;
    JanetFunction *fun = get_dunder_function(abs, "__put");
    if (fun != NULL) {
        Janet argv[] = {janet_wrap_table(abs->t), key, val};
        janet_call(fun, 3, argv);
    } else {
        janet_table_put(abs->t, key, val);
    }
}

/* Stringifying */

static void abstract_to_string(void *p, JanetBuffer *buf) {
    abstract_t *abs = (abstract_t *)p;
    JanetFunction *fun = get_dunder_function(abs, "__tostring");
    if (fun != NULL) {
        Janet argv[] = {janet_wrap_abstract(abs), janet_wrap_buffer(buf)};
        janet_call(fun, 2, argv);
    } else {
        janet_buffer_push_string(buf, janet_description(janet_wrap_table(abs->t)));
    }
}

static int abstract_compare(void *p1, void *p2) {
    abstract_t *a = (abstract_t *)p1;
    abstract_t *b = (abstract_t *)p2;
    JanetFunction *fun = get_dunder_function(a, "__compare");
    if (fun == NULL)
        janet_panicf("object does not support the __compare method");
    Janet argv[] = {janet_wrap_table(a->t), janet_wrap_table(b->t)};
    Janet r = janet_call(fun, 2, argv);
    if (!janet_checktype(r, JANET_NUMBER))
        janet_panicf("invalid return value from __compare");
    return (int) janet_unwrap_number(r);
}

static Janet abstract_next(void *p, Janet key) {
    abstract_t *abs = (abstract_t *)p;
    JanetFunction *fun = get_dunder_function(abs, "__next");
    if (fun != NULL) {
        Janet argv[] = {janet_wrap_table(abs->t), key};
        return janet_call(fun, 2, argv);
    } 
    
    return janet_wrap_nil();
}

/* Type Definition */

static const JanetAbstractType abstract_type = {
    "abstract/new",
    abstract_gc,
    abstract_mark, /* GC Mark */
    abstract_get, /* Get */
    abstract_put, /* Put */
    NULL, /* Marshall */
    NULL, /* Unmarshall */
    abstract_to_string, /* String */
    abstract_compare, /* Compare */
    NULL, //abstract_hash
    abstract_next, /* Next */
    JANET_ATEND_NEXT
};

/* C Functions */

static Janet cfun_abstract_new(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 1);
    JanetTable *t = janet_gettable(argv, 0);
    abstract_t *abs = (abstract_t *)janet_abstract(&abstract_type, sizeof(abstract_t));
    abs->t = t;
    return janet_wrap_abstract(abs);
}


static const JanetReg cfuns[] = {
    {"new", cfun_abstract_new, NULL},
    {NULL, NULL, NULL}
};

void abstract_register_type(JanetTable *env) {
    (void) env;
    janet_register_abstract_type(&abstract_type);
}

void abstract_register_functions(JanetTable *env) {
    janet_cfuns(env, "abstract", cfuns);
}

JANET_MODULE_ENTRY(JanetTable *env) {
    abstract_register_type(env);
    abstract_register_functions(env);
}
