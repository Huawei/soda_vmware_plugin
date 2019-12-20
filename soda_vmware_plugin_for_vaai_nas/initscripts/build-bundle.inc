
VIB_TESTCERTS := $(VIBPATH)/testcerts

CERTIFIED_CERTPATH = CERTPATH=$(VIB_TESTCERTS)/vmware.cert
CERTIFIED_KEYPATH = KEYPATH=$(VIB_TESTCERTS)/vmware.key

ACCEPTED_CERTPATH = CERTPATH=$(VIB_TESTCERTS)/accepted.cert
ACCEPTED_KEYPATH = KEYPATH=$(VIB_TESTCERTS)/accepted.key

PARTNER_CERTPATH = CERTPATH=$(VIB_TESTCERTS)/partner.cert
PARTNER_KEYPATH = KEYPATH=$(VIB_TESTCERTS)/partner.key

vib-bundle-safe:
	@ac=`$(EGREP) "<acceptance-level>.*</acceptance-level>" $(VIB_DESCRIPTOR_XML) | $(SED) -e "s@.*<acceptance-level>\(.*\)</acceptance-level>.*@\1@"`; \
        if [ "$${ac}" == "community" ] ; \
        then \
           $(MAKE) -f Makefile vib-bundle ; \
        elif [ "$${ac}" == "certified" ] ; \
        then \
           $(CERTIFIED_CERTPATH) $(CERTIFIED_KEYPATH) $(MAKE) -f Makefile vib-bundle ; \
        elif [ "$${ac}" == "accepted" ] ; \
        then \
           $(ACCEPTED_CERTPATH) $(ACCEPTED_KEYPATH) $(MAKE) -f Makefile vib-bundle ; \
        elif [ "$${ac}" == "partner" ] ; \
        then \
           $(PARTNER_CERTPATH) $(PARTNER_KEYPATH) $(MAKE) -f Makefile vib-bundle ; \
        else \
           echo 'Error: Bad acceptance level is specified in the descriptor file [$(VIB_DESCRIPTOR_XML)]' 1>&2 ; \
           exit 1; \
        fi;

