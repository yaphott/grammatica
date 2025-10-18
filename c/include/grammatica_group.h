#ifndef GRAMMATICA_GROUP_H
#define GRAMMATICA_GROUP_H

#include "grammatica.h"

#ifdef __cplusplus
extern "C" {
#endif

char* render_quantifier(Quantifier quantifier);

bool quantifier_needs_wrap(Quantifier quantifier);

#ifdef __cplusplus
}
#endif

#endif /* GRAMMATICA_GROUP_H */
