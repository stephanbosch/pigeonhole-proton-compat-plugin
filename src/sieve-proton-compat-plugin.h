#ifndef SIEVE_PROTON_COMPAT_PLUGIN_H
#define SIEVE_PROTON_COMPAT_PLUGIN_H

/*
 * Plugin interface
 */

void sieve_proton_compat_plugin_load(struct sieve_instance *svinst,
				     void **context_r);
void sieve_proton_compat_plugin_unload(struct sieve_instance *svinst,
				       void *context);

/*
 * Module interface
 */

void sieve_proton_compat_plugin_init(void);
void sieve_proton_compat_plugin_deinit(void);

#endif
