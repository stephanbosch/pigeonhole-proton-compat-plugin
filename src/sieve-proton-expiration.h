#ifndef SIEVE_PROTON_EXPIRATION_H
#define SIEVE_PROTON_EXPIRATION_H

void sieve_proton_expiration_set(const struct sieve_runtime_env *renv,
				 sieve_number_t expiration);
void sieve_proton_expiration_unset(const struct sieve_runtime_env *renv);

bool sieve_proton_expiration_is_set(const struct sieve_runtime_env *renv);

sieve_number_t
sieve_proton_expiration_get(const struct sieve_runtime_env *renv);

#endif
