PL = sicstus
SPLD = spld
SPLDFLAGS = --static --exechome=/opt/sicstus4.2.0/bin/

ALL = verify

all: $(ALL)

%: %.sav
	$(SPLD) $(SPLDFLAGS) $< -o $@ 

%.sav: %.pl
	echo "compile('$<'). save_program('$@')." | $(PL)

clean:
	rm -f $(ALL) *.sav

