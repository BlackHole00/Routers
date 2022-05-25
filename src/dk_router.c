#include "dk_router.h"

#include <string.h>

dk_Router dk_router_from_identifier(char* identifier) {
    int len = strlen(identifier) + 1;

    dk_Router router = VX_DEFAULT(dk_Router);
    router.identifier = vx_smalloc(sizeof(char) * len);
    strcpy(router.identifier, identifier);

    return router;
}

void dk_router_free(dk_Router* router) {
    vx_free(router->identifier);
}