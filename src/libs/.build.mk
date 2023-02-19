GENERATED_MOD += $(patsubst sources/libs/%, $(GENDIR)/headers/%, $(wildcard sources/libs/*))

.SECONDARY: $(GENERATED_MOD)
$(GENDIR)/headers/%: sources/libs/%
	@$(MKCWD)
	./sources/build/scripts/header-builder.py -d $< --prefix $(shell basename $<) > $@
