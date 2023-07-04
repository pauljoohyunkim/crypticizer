#ifndef CRYPTICIZER_H
#define CRYPTICIZER_H

#define CRYPTICIZER ".crypticizer"
#define HASHFILE "shadow"
#define HASHFUNCTION HFT_SHA512
#define HASH_SALT_N_BYTES 16

#include "session.h"

extern Session crypticizerSession;

#endif
