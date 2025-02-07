/* Copyright (c) 2025 Pigeonhole authors, see the included COPYING file */

/* Extension vnd.proton.expire
   ---------------------------

   Authors: Stephan Bosch
   Specification: vendor-defined; unavailable
   Implementation: dummy
   Status: testing
 */

#include "sieve-common.h"
#include "sieve-extensions.h"
#include "sieve-commands.h"
#include "sieve-validator.h"

#include "sieve-proton-compat-common.h"

/*
 * Operations
 */

const struct sieve_operation_def *expire_operations[] = {
	&vprn_expire_operation,
	&vprn_unexpire_operation,
	&vprn_expiration_operation,
	&vprn_hasexpiration_operation,
};

/*
 * Extension
 */

static bool
ext_expire_validator_load(const struct sieve_extension *ext,
			  struct sieve_validator *valdtr);

const struct sieve_extension_def vnd_proton_expire_extension = {
	.name = "vnd.proton.expire",
	.validator_load = ext_expire_validator_load,
	SIEVE_EXT_DEFINE_OPERATIONS(expire_operations),
};

static bool
ext_expire_validator_load(const struct sieve_extension *ext,
			  struct sieve_validator *valdtr)
{
	/* Register new commands */
	sieve_validator_register_command(valdtr, ext, &vprn_expire_command);
	sieve_validator_register_command(valdtr, ext, &vprn_unexpire_command);
	sieve_validator_register_command(valdtr, ext, &vprn_expiration_test);
	sieve_validator_register_command(valdtr, ext, &vprn_hasexpiration_test);
	return TRUE;
}
