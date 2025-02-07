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
     expiration :comparator "i;ascii-numeric" [MATCH-TYPE]
       <unit: "day" / "minute" / "second"> <key-list: string-list>
 */

static bool
tst_expiration_registered(struct sieve_validator *valdtr,
			  const struct sieve_extension *ext,
			  struct sieve_command_registration *cmd_reg);
static bool
tst_expiration_validate(struct sieve_validator *valdtr,
			struct sieve_command *tst);
static bool
tst_expiration_generate(const struct sieve_codegen_env *cgenv,
			struct sieve_command *tst);

const struct sieve_command_def vprn_expiration_test = {
	.identifier = "expiration",
	.type = SCT_TEST,
	.positional_args = 2,
	.subtests = 0,
	.block_allowed = FALSE,
	.block_required = FALSE,
	.registered = tst_expiration_registered,
	.validate = tst_expiration_validate,
	.generate = tst_expiration_generate,
};

/*
 * Header operation
 */

static bool
tst_expiration_operation_dump(const struct sieve_dumptime_env *denv,
			      sieve_size_t *address);
static int
tst_expiration_operation_execute(const struct sieve_runtime_env *renv,
				 sieve_size_t *address);

const struct sieve_operation_def vprn_expiration_operation = {
	.mnemonic = "EXPIRATION",
	.ext_def = &vnd_proton_expire_extension,
	.code = VPRN_EXPIRE_OPERATION_EXPIRATION,
	.dump = tst_expiration_operation_dump,
	.execute = tst_expiration_operation_execute
};

/*
 * Test registration
 */

static bool
tst_expiration_registered(struct sieve_validator *valdtr,
			  const struct sieve_extension *ext ATTR_UNUSED,
			  struct sieve_command_registration *cmd_reg)
{
	/* The order of these is not significant */
	sieve_comparators_link_tag(
		valdtr, cmd_reg, SIEVE_MATCH_OPT_COMPARATOR);
	sieve_match_types_link_tags(
		valdtr, cmd_reg, SIEVE_MATCH_OPT_MATCH_TYPE);

	return TRUE;
}

/*
 * Validation
 */

static bool
tst_expiration_validate(struct sieve_validator *valdtr,
			struct sieve_command *tst)
{
	struct sieve_ast_argument *arg = tst->first_positional;
	struct sieve_comparator cmp_default =
		SIEVE_COMPARATOR_DEFAULT(i_octet_comparator);
	struct sieve_match_type mcht_default =
		SIEVE_MATCH_TYPE_DEFAULT(is_match_type);

	if (!sieve_validate_positional_argument(
		valdtr, tst, arg, "unit", 1, SAAT_STRING))
		return FALSE;
	if (!sieve_validator_argument_activate(valdtr, tst, arg, FALSE))
		return FALSE;

	if (sieve_argument_is_string_literal(arg)) {
		const char *unit = sieve_ast_argument_strc(arg);

		if (strcmp(unit, "day") != 0 && strcmp(unit, "minute") != 0 &&
		    strcmp(unit, "second") != 0) {
			sieve_argument_validate_error(
				valdtr, arg, "expiration test: "
				"specified unit '%s' is invalid", unit);
			return FALSE;
		}
	}

	arg = sieve_ast_argument_next(arg);

	if (!sieve_validate_positional_argument(
		valdtr, tst, arg, "key-list", 2, SAAT_STRING_LIST))
		return FALSE;

	if (!sieve_validator_argument_activate(valdtr, tst, arg, FALSE))
		return FALSE;

	/* Validate the key argument to a specified match type */
	return sieve_match_type_validate(valdtr, tst, arg,
					 &mcht_default, &cmp_default);
}

/*
 * Code generation
 */

static bool
tst_expiration_generate(const struct sieve_codegen_env *cgenv,
			struct sieve_command *tst)
{
	sieve_operation_emit(cgenv->sblock, tst->ext,
			     &vprn_expiration_operation);

	/* Generate arguments */
	return sieve_generate_arguments(cgenv, tst, NULL);
}

/*
 * Code dump
 */

static bool
tst_expiration_operation_dump(const struct sieve_dumptime_env *denv,
			      sieve_size_t *address)
{
	sieve_code_dumpf(denv, "EXPIRATION");
	sieve_code_descend(denv);

	/* Optional operands */
	if (sieve_match_opr_optional_dump(denv, address, NULL) != 0)
		return FALSE;

	return (sieve_opr_string_dump(denv, address, "unit") &&
		sieve_opr_stringlist_dump(denv, address, "key list"));
}

/*
 * Expire stringlist
 */

/* Forward declarations */

static int
ext_expire_stringlist_next_item(struct sieve_stringlist *_strlist,
				string_t **str_r);
static void ext_expire_stringlist_reset(struct sieve_stringlist *_strlist);

/* Stringlist object */

struct ext_expire_stringlist {
	struct sieve_stringlist strlist;

	const char *expiration_str;

	bool read:1;
};

static struct sieve_stringlist *
ext_expire_stringlist_create(const struct sieve_runtime_env *renv,
			     const char *unit)
{
	struct ext_expire_stringlist *strlist;
	sieve_number_t expiration = sieve_proton_expiration_get(renv);

	strlist = t_new(struct ext_expire_stringlist, 1);
	strlist->strlist.runenv = renv;
	strlist->strlist.exec_status = SIEVE_EXEC_OK;
	strlist->strlist.next_item = ext_expire_stringlist_next_item;
	strlist->strlist.reset = ext_expire_stringlist_reset;

	if (strcmp(unit, "day") == 0)
		expiration /= 60 * 60 * 24;
	else if (strcmp(unit, "minute") == 0)
		expiration /= 60;
	else
		i_assert(strcmp(unit, "second") == 0);

	strlist->expiration_str = t_strdup_printf("%"PRIu64, expiration);

	return &strlist->strlist;
}

/* Stringlist implementation */

static int
ext_expire_stringlist_next_item(struct sieve_stringlist *_strlist,
				string_t **str_r)
{
	struct ext_expire_stringlist *strlist =
		container_of(_strlist, struct ext_expire_stringlist, strlist);

	/* Check whether the item was already read */
	if (strlist->read)
		return 0;

	*str_r = t_str_new_const(strlist->expiration_str,
				 strlen(strlist->expiration_str));
	strlist->read = TRUE;
	return 1;
}

static void ext_expire_stringlist_reset(struct sieve_stringlist *_strlist)
{
	struct ext_expire_stringlist *strlist =
		container_of(_strlist, struct ext_expire_stringlist, strlist);

	strlist->read = FALSE;
}

/*
 * Code execution
 */

static int
tst_expiration_operation_execute(const struct sieve_runtime_env *renv,
				 sieve_size_t *address)
{
	struct sieve_comparator cmp =
		SIEVE_COMPARATOR_DEFAULT(i_ascii_casemap_comparator);
	struct sieve_match_type mcht =
		SIEVE_MATCH_TYPE_DEFAULT(is_match_type);
	string_t *unit;
	struct sieve_stringlist *value_list, *key_list;
	int match, ret;

	/*
	 * Read operands
	 */

	/* Optional operands */
	if (sieve_match_opr_optional_read(renv, address, NULL,
					  &ret, &cmp, &mcht) < 0 )
		return ret;

	/* Read header-list */
	ret = sieve_opr_string_read(renv, address, "unit", &unit);
	if (ret <= 0)
		return ret;

	/* Read key-list */
	ret = sieve_opr_stringlist_read(renv, address, "key-list", &key_list);
	if (ret <= 0)
		return ret;

	/*
	 * Check operands
	 */

	if (strcmp(str_c(unit), "day") != 0 &&
	    strcmp(str_c(unit), "minute") != 0 &&
	    strcmp(str_c(unit), "second") != 0) {
		sieve_runtime_error(renv, NULL, "expiration test: "
				    "specified unit '%s' is invalid",
				    str_sanitize(str_c(unit), 32));
		return SIEVE_EXEC_FAILURE;
	}

	/*
	 * Perform test
	 */

	sieve_runtime_trace(renv, SIEVE_TRLVL_TESTS, "expiration test");

	value_list = ext_expire_stringlist_create(renv, str_c(unit));

	/* Perform match */
	match = sieve_match(renv, &mcht, &cmp, value_list, key_list, &ret);
	if (match < 0)
		return ret;

	/* Set test result for subsequent conditional jump */
	sieve_interpreter_set_test_result(renv->interp, match > 0);
	return SIEVE_EXEC_OK;
}
