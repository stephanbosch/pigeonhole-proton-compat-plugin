/* Copyright (c) 2025 Pigeonhole authors, see the included COPYING file */

#include "lib.h"
#include "array.h"

#include "sieve-common.h"
#include "sieve-error.h"
#include "sieve-extensions.h"

#include "sieve-proton-compat-common.h"
#include "sieve-proton-compat-plugin.h"

/*
 * Proton extensions
 */

static const struct sieve_extension_def *sieve_proton_extensions[] = {
	&vnd_proton_expire_extension,
};

const unsigned int sieve_proton_extensions_count =
	N_ELEMENTS(sieve_proton_extensions);

/*
 * Sieve plugin interface
 */

struct _plugin_context {
	const struct sieve_extension **exts;
};

const char *sieve_proton_compat_plugin_version = PIGEONHOLE_ABI_VERSION;

void sieve_proton_compat_plugin_load(struct sieve_instance *svinst,
				     void **context_r)
{
	/* Register vnd.proton.* extensions */

	const struct sieve_extension **exts;
	unsigned int i;

	exts = i_new(const struct sieve_extension *,
		     sieve_proton_extensions_count);

	for (i = 0; i < sieve_proton_extensions_count; i++) {
		exts[i] = sieve_extension_register(
			svinst, sieve_proton_extensions[i], FALSE);
		i_assert(exts[i] != NULL);
		i_assert(str_begins_with(sieve_extension_name(exts[i]),
					 "vnd.proton."));
	}

	/* Create plugin context */

	struct _plugin_context *pctx;

	pctx = i_new(struct _plugin_context, 1);
	pctx->exts = exts;

	/* Initialize */

	e_debug(svinst->event, "%s version %s loaded",
		SIEVE_PROTON_COMPAT_NAME, SIEVE_PROTON_COMPAT_VERSION);

	*context_r = pctx;
}

void sieve_proton_compat_plugin_unload(
	struct sieve_instance *svinst ATTR_UNUSED, void *context)
{
	struct _plugin_context *pctx = (struct _plugin_context *)context;
	unsigned int i;

	for (i = 0; i < sieve_proton_extensions_count; i++)
		sieve_extension_unregister(pctx->exts[i]);

	i_free(pctx->exts);
	i_free(pctx);
}

/*
 * Module interface
 */

void sieve_proton_compat_plugin_init(void)
{
	/* Nothing */
}

void sieve_proton_compat_plugin_deinit(void)
{
	/* Nothing */
}
