if BUILD_DOCS
DOCS = doc
endif

SUBDIRS = src $(DOCS)

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
	version \
	tests \
	ChangeLog

dist-hook:
	rm -rf `find $(distdir)/tests -type f -name '*.svbin'`

ChangeLog:
	git log --name-status \
		--pretty="format:%ai %aN <%aE> (%h)%n%n%w(80,4,4)%s%n%n%b" > ChangeLog \
			|| rm -f ChangeLog

if PIGEONHOLE_TESTSUITE_AVAILABLE

TESTSUITE_PLUGIN = -P src/.libs/sieve_proton_compat

TESTSUITE_BIN = $(PIGEONHOLE_TESTSUITE) \
	$(TESTSUITE_PLUGIN) \
	$(TESTSUITE_OPTIONS)

TEST_BIN = $(RUN_TEST) $(TESTSUITE_BIN)

test_cases = \
	tests/expire/basic.svtest

$(test_cases):
	@$(TEST_BIN) $@
$(test_cases_bare):
	@$(TEST_BIN_BARE) $@

.PHONY: $(test_cases) $(test_cases_bare)
test: $(test_cases) $(test_cases_bare)

check: check-am test all-am

else

test:
	@echo "Pigeonhole test suite not compiled."

endif

version:
	$(AM_V_GEN)$(top_srcdir)/build-aux/git-version-gen > $@
