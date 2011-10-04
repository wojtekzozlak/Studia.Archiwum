#include <stdlib.h>
#include <stdio.h>
#include <xmlrpc.h>
#include <xmlrpc-c/server.h>
#include <xmlrpc-c/server_abyss.h>

static xmlrpc_value *
sumAndDifference(xmlrpc_env * const envP, xmlrpc_value * const paramArrayP,
           void * const serverContext)
{
    xmlrpc_int32 a, b, sum, diff;
    printf("Computing output\n");

    xmlrpc_parse_value(envP, paramArrayP, "(ii)", &a, &b);
    if (envP->fault_occurred)
        return NULL;

    sum = a + b;
    diff = a - b;

    return xmlrpc_build_value(envP, "{s:i,s:i}", "sum", sum, "difference", diff);
}


int
main (int const argc, const char ** const argv)
{
    xmlrpc_registry * registryP;
    xmlrpc_env env;
    TServer abyssServer;

    xmlrpc_env_init(&env);
    registryP = xmlrpc_registry_new(&env);
    xmlrpc_registry_add_method(&env, registryP, NULL,
			"sample.sumAndDifference", &sumAndDifference, NULL);

    ServerCreate(&abyssServer, "XmlRpcServer", 5763, DEFAULT_DOCS, NULL);
    xmlrpc_server_abyss_set_handlers2(&abyssServer, "/", registryP);

    ServerInit(&abyssServer);
    ServerRun(&abyssServer);

    return 0;
}
