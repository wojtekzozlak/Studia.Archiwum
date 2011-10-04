#include <rpc/rpc.h>
#include "sad.h"

intpair * sad_1_svc(intpair *i, struct svc_req *rqstp)
{
	static intpair result;
	result.a = i->a + i->b;
	result.b = i->a - i->b;
	return &result;
};
