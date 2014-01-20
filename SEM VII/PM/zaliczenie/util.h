#ifndef _SMS_UTIL_
#define _SMS_UTIL_

#define SBI(where, bit) where |= 1 << (bit)
#define CBI(where, bit) where &= ~(1 << (bit))

#endif

