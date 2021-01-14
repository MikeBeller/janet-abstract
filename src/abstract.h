#ifndef ABSTRACT_H
#define ABSTRACT_H

#include <janet.h>

void abstract_register_type(JanetTable *env);
void abstract_register_functions(JanetTable *env);

#endif /* end of include guard */
