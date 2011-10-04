#include <xmlrpc.h>
#include <xmlrpc_client.h>
#include <stdio.h>
#include <stdlib.h>

#define NAME "XML-RPC C Test Client"
#define VERSION "1.0"

void die_if_fault_occurred (xmlrpc_env *env)
{
    if (env->fault_occurred)
    {
        fprintf(stderr, "XML-RPC Fault: %s (%d)\n",
        env->fault_string, env->fault_code);
        exit(1);
    };
}

int
main(int const argc, const char ** const argv)
{
    xmlrpc_env env;
    xmlrpc_value * resultP;
    xmlrpc_int32 a, b;
    int sum, difference;
    char * const url = "http://localhost:5763/";
    char * const methodName = "sample.sumAndDifference";

    printf("Specify two integers to process:\n");
    scanf("%d %d", &a, &b);

    /* Initialize our error-handling environment. */
    xmlrpc_env_init(&env);

    /* Start up our XML-RPC client library. */
    xmlrpc_client_init2(&env, XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION, NULL, 0);
    die_if_fault_occurred(&env);


    resultP = xmlrpc_client_call(&env, url, methodName,
                 "(ii)", a, b);
    die_if_fault_occurred(&env);


    xmlrpc_parse_value(&env, resultP, "{s:i,s:i,*}", "sum", &sum,
					"difference", &difference);
    die_if_fault_occurred(&env);
    printf("Sum: %d\n", (int) sum);
    printf("Difference: %d\n", (int) difference);

    xmlrpc_DECREF(resultP);
    xmlrpc_env_clean(&env);
    xmlrpc_client_cleanup();

    return 0;
}
