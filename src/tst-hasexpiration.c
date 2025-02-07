/* Copyright (c) 2025 Pigeonhole authors, see the included COPYING file */

#include "lib.h"
#include "str.h"
#include "str-sanitize.h"
#include "istream.h"
#include "message-parser.h"
#include "message-header-decode.h"
#include "mail-storage.h"

#include "sieve-common.h"
#include "sieve-stringlist.h"
#include "sieve-commands.h"
#include "sieve-comparators.h"
#include "sieve-match-types.h"
#include "sieve-validator.h"
#include "sieve-generator.h"
#include "sieve-interpreter.h"
#include "sieve-code.h"
#include "sieve-binary.h"
#include "sieve-message.h"
#include "sieve-dump.h"
#include "sieve-match.h"

#include "sieve-proton-compat-common.h"
#include "sieve-proton-expiration.h"

/*
   Expire test

   Syntax:
     hasexpiration
 */

static bool
tst_hasexpiration_generate(const struct sieve_codegen_env *cgenv,
			   struct sieve_command *tst);

const struct sieve_command_def vprn_hasexpiration_test = {
	.identifier = "hasexpiration",
	.type = SCT_TEST,
	.positional_args = 0,
	.subtests = 0,
	.block_allowed = FALSE,
	.block_required = FALSE,
	.generate = tst_hasexpiration_generate,
};

/*
 * Header operation
 */

static int
tst_hasexpiration_operation_execute(const struct sieve_runtime_env *renv,
				    sieve_size_t *address);

const struct sieve_operation_def vprn_hasexpiration_operation = {
	.mnemonic = "HASEXPIRATION",
	.ext_def = &vnd_proton_expire_extension,
	.code = VPRN_EXPIRE_OPERATION_HASEXPIRATION,
	.execute = tst_hasexpiration_operation_execute
};

/*
 * Code generation
 */

static bool
tst_hasexpiration_generate(const struct sieve_codegen_env *cgenv,
			   struct sieve_command *tst)
{
	sieve_operation_emit(cgenv->sblock, tst->ext,
			     &vprn_hasexpiration_operation);

	/* Generate arguments */
	return sieve_generate_arguments(cgenv, tst, NULL);
}

/*
 * Code execution
 */

static int
tst_hasexpiration_operation_execute(const struct sieve_runtime_env *renv,
				    sieve_size_t *address ATTR_UNUSED)
{
	sieve_runtime_trace(renv, SIEVE_TRLVL_TESTS, "hasexpiration test");

	/* Set test result for subsequent conditional jump */
	sieve_interpreter_set_test_result(renv->interp,
					  sieve_proton_expiration_is_set(renv));
	return SIEVE_EXEC_OK;
}
