/* Copyright (c) 2025 Pigeonhole authors, see the included COPYING file */

#include "lib.h"
#include "str-sanitize.h"

#include "sieve-common.h"
#include "sieve-stringlist.h"
#include "sieve-extensions.h"
#include "sieve-commands.h"
#include "sieve-validator.h"
#include "sieve-generator.h"
#include "sieve-interpreter.h"
#include "sieve-runtime.h"
#include "sieve-code.h"
#include "sieve-binary.h"

#include "sieve-proton-compat-common.h"
#include "sieve-proton-expiration.h"

/*
   Unexpire command

   Syntax:
     unexpire
 */

static bool
cmd_unexpire_generate(const struct sieve_codegen_env *cgenv,
		      struct sieve_command *ctx);

const struct sieve_command_def vprn_unexpire_command = {
	.identifier = "unexpire",
	.type = SCT_COMMAND,
	.positional_args = 0,
	.subtests = 0,
	.block_allowed = FALSE,
	.block_required = FALSE,
	.generate = cmd_unexpire_generate
};

/*
 * Test operation
 */

static int
cmd_unexpire_operation_execute(const struct sieve_runtime_env *renv,
			       sieve_size_t *address);

const struct sieve_operation_def vprn_unexpire_operation = {
	.mnemonic = "UNEXPIRE",
	.ext_def = &vnd_proton_expire_extension,
	.code = VPRN_EXPIRE_OPERATION_UNEXPIRE,
	.execute = cmd_unexpire_operation_execute
};

/*
 * Code generation
 */

static bool
cmd_unexpire_generate(const struct sieve_codegen_env *cgenv,
		      struct sieve_command *cmd)
{
	sieve_operation_emit(cgenv->sblock, cmd->ext, &vprn_unexpire_operation);

	/* Generate arguments */
	return sieve_generate_arguments(cgenv, cmd, NULL);
}

/*
 * Intepretation
 */

static int
cmd_unexpire_operation_execute(const struct sieve_runtime_env *renv,
			       sieve_size_t *address ATTR_UNUSED)
{
	sieve_runtime_trace(renv, SIEVE_TRLVL_ACTIONS, "unexpire command");

	sieve_proton_expiration_unset(renv);
	return SIEVE_EXEC_OK;
}
