/* Copyright (c) 2025 Pigeonhole authors, see the included COPYING file */

#include "lib.h"
#include "hash.h"
#include "array.h"
#include "ioloop.h"
#include "dict.h"
#include "settings.h"
#include "message-address.h"
#include "mail-storage.h"

#include "sieve-common.h"
#include "sieve-code.h"
#include "sieve-extensions.h"
#include "sieve-message.h"
#include "sieve-runtime.h"

#include "sieve-proton-compat-common.h"
#include "sieve-proton-expiration.h"

struct ext_expire_context {
	sieve_number_t expiration;
};

/*
 * Runtime initialization
 */

static struct ext_expire_context *
sieve_proton_expire_get_context(const struct sieve_runtime_env *renv)
{
	const struct sieve_extension *ext = renv->oprtn->ext;
	struct ext_expire_context *exctx;
	pool_t pool;

	i_assert(sieve_extension_is(ext, vnd_proton_expire_extension));

	exctx = (struct ext_expire_context *)
		sieve_message_context_extension_get(renv->msgctx, ext);
	if (exctx == NULL) {
		/* Create context */
		pool = sieve_message_context_pool(renv->msgctx);
		exctx = p_new(pool, struct ext_expire_context, 1);
		sieve_message_context_extension_set(renv->msgctx, ext, exctx);
	}

	return exctx;
}

void sieve_proton_expiration_set(const struct sieve_runtime_env *renv,
				 sieve_number_t expiration)
{
	struct ext_expire_context *exctx =
		sieve_proton_expire_get_context(renv);

	exctx->expiration = expiration;
}

void sieve_proton_expiration_unset(const struct sieve_runtime_env *renv)
{
	struct ext_expire_context *exctx =
		sieve_proton_expire_get_context(renv);

	exctx->expiration = 0;
}

bool sieve_proton_expiration_is_set(const struct sieve_runtime_env *renv)
{
	struct ext_expire_context *exctx =
		sieve_proton_expire_get_context(renv);

	return (exctx->expiration != 0);
}

sieve_number_t
sieve_proton_expiration_get(const struct sieve_runtime_env *renv)
{
	struct ext_expire_context *exctx =
		sieve_proton_expire_get_context(renv);

	return exctx->expiration;
}
