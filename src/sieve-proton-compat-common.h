#ifndef SIEVE_PROTON_COMPAT_COMMON_H
#define SIEVE_PROTON_COMPAT_COMMON_H

#include "sieve-common.h"
#include "sieve-proton-compat-config.h"

/*
 * Commands
 */

extern const struct sieve_command_def vprn_expire_command;
extern const struct sieve_command_def vprn_unexpire_command;

extern const struct sieve_command_def vprn_expiration_test;
extern const struct sieve_command_def vprn_hasexpiration_test;

/*
 * Operations
 */

enum vprn_expire_opcode {
	VPRN_EXPIRE_OPERATION_EXPIRE,
	VPRN_EXPIRE_OPERATION_UNEXPIRE,
	VPRN_EXPIRE_OPERATION_EXPIRATION,
	VPRN_EXPIRE_OPERATION_HASEXPIRATION,
};

extern const struct sieve_operation_def vprn_expire_operation;
extern const struct sieve_operation_def vprn_unexpire_operation;

extern const struct sieve_operation_def vprn_expiration_operation;
extern const struct sieve_operation_def vprn_hasexpiration_operation;

/*
 * Extensions
 */

extern const struct sieve_extension_def vnd_proton_expire_extension;

#endif
