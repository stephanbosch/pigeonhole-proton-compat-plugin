/* Copyright (c) 2025 Pigeonhole authors, see the included COPYING file */

#include "lib.h"
#include "strnum.h"
#include "str-sanitize.h"

#include "sieve-common.h"
#include "sieve-stringlist.h"
#include "sieve-extensions.h"
#include "sieve-commands.h"
#include "sieve-validator.h"
#include "sieve-generator.h"
#include "sieve-code.h"
#include "sieve-binary.h"
#include "sieve-interpreter.h"

#include "sieve-proton-compat-common.h"
#include "sieve-proton-expiration.h"

/*
 * Unexpire command
 *
 * Syntax:
 *   expire <unit: "day" / "minute" / "second"> <value: string>
 */

static bool
cmd_expire_validate(struct sieve_validator *valdtr, struct sieve_command *cmd);
static bool
cmd_expire_generate(const struct sieve_codegen_env *cgenv,
		    struct sieve_command *ctx);

const struct sieve_command_def vprn_expire_command = {
	.identifier = "expire",
	.type = SCT_COMMAND,
	.positional_args = 2,
	.subtests = 0,
	.block_allowed = FALSE,
	.block_required = FALSE,
	.validate = cmd_expire_validate,
	.generate = cmd_expire_generate
};

/*
 * Test operation
 */

static bool
cmd_expire_operation_dump(const struct sieve_dumptime_env *denv,
			  sieve_size_t *address);
static int
cmd_expire_operation_execute(const struct sieve_runtime_env *renv,
			     sieve_size_t *address);

const struct sieve_operation_def vprn_expire_operation = {
	.mnemonic = "EXPIRE",
	.ext_def = &vnd_proton_expire_extension,
	.code = VPRN_EXPIRE_OPERATION_EXPIRE,
	.dump = cmd_expire_operation_dump,
	.execute = cmd_expire_operation_execute
};

/*
 * Validation
 */

static bool
cmd_expire_validate(struct sieve_validator *valdtr, struct sieve_command *cmd)
{
	struct sieve_ast_argument *arg = cmd->first_positional;

	if (!sieve_validate_positional_argument(valdtr, cmd, arg, "unit", 1,
						SAAT_STRING))
		return FALSE;
	if (!sieve_validator_argument_activate(valdtr, cmd, arg, FALSE))
		return FALSE;

	if (sieve_argument_is_string_literal(arg)) {
		const char *unit = sieve_ast_argument_strc(arg);

		if (strcmp(unit, "day") != 0 && strcmp(unit, "minute") != 0 &&
		    strcmp(unit, "second") != 0) {
			sieve_argument_validate_error(
				valdtr, arg, "expire command: "
				"specified unit '%s' is invalid", unit);
			return FALSE;
		}
	}
	arg = sieve_ast_argument_next(arg);

	if (!sieve_validate_positional_argument(valdtr, cmd, arg, "value", 2,
						SAAT_STRING))
		return FALSE;
	if (!sieve_validator_argument_activate(valdtr, cmd, arg, FALSE))
		return FALSE;

	if (sieve_argument_is_string_literal(arg)) {
		const char *value = sieve_ast_argument_strc(arg);
		sieve_number_t expiration;

		if (str_to_uint64(value, &expiration) < 0) {
			sieve_argument_validate_error(
				valdtr, arg, "expire command: "
				"specified value '%s' is invalid", value);
			return FALSE;
		}
	}

	return TRUE;
}

/*
 * Code generation
 */

static bool
cmd_expire_generate(const struct sieve_codegen_env *cgenv,
		    struct sieve_command *cmd)
{
	sieve_operation_emit(cgenv->sblock, cmd->ext, &vprn_expire_operation);

	/* Generate arguments */
	return sieve_generate_arguments(cgenv, cmd, NULL);
}

/*
 * Code dump
 */

static bool
cmd_expire_operation_dump(const struct sieve_dumptime_env *denv,
			  sieve_size_t *address)
{
	sieve_code_dumpf(denv, "EXPIRE:");
	sieve_code_descend(denv);

	return (sieve_opr_string_dump(denv, address, "unit") &&
		sieve_opr_string_dump(denv, address, "value"));
}

/*
 * Intepretation
 */

static int
cmd_expire_operation_execute(const struct sieve_runtime_env *renv,
			     sieve_size_t *address)
{
	string_t *unit, *value;
	bool trace = sieve_runtime_trace_active(renv, SIEVE_TRLVL_ACTIONS);
	int ret;

	/*
	 * Read operands
	 */

	ret = sieve_opr_string_read(renv, address, "unit", &unit);
	if (ret <= 0)
		return ret;
	ret = sieve_opr_string_read(renv, address, "value", &value);
	if (ret <= 0)
		return ret;

	/*
	 * Perform operation
	 */

	sieve_number_t expiration;

	if (str_to_uint64(str_c(value), &expiration) < 0) {
		sieve_runtime_error(renv, NULL, "expire command: "
				    "specified value '%s' is invalid",
				    str_sanitize(str_c(value), 32));
		return SIEVE_EXEC_FAILURE;
	}

	if (strcmp(str_c(unit), "day") == 0)
		expiration *= 60 * 60 * 24;
	else if (strcmp(str_c(unit), "minute") == 0)
		expiration *= 60;
	else if (strcmp(str_c(unit), "second") != 0) {
		sieve_runtime_error(renv, NULL, "expire command: "
				    "specified unit '%s' is invalid",
				    str_sanitize(str_c(unit), 32));
		return SIEVE_EXEC_FAILURE;
	}

	if (trace) {
		sieve_runtime_trace(renv, SIEVE_TRLVL_ACTIONS, "expire action");
		sieve_runtime_trace_descend(renv);
		sieve_runtime_trace(renv, 0, "set expiry to %"PRIu64,
				    expiration);
	}

	sieve_proton_expiration_set(renv, expiration);
	return SIEVE_EXEC_OK;
}
