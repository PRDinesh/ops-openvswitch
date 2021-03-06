# Install all the OVS header files under 
# /usr/include/ovs directory to avoid
# name collision with standard headers.
if OPS
openflowincludedir = $(includedir)/ovs/openflow
else
openflowincludedir = $(includedir)/openflow
endif
openflowinclude_HEADERS = \
	include/openflow/nicira-ext.h \
	include/openflow/openflow-1.0.h \
	include/openflow/openflow-1.1.h \
	include/openflow/openflow-1.2.h \
	include/openflow/openflow-1.3.h \
	include/openflow/openflow-1.4.h \
	include/openflow/openflow-1.5.h \
	include/openflow/openflow-common.h \
	include/openflow/openflow.h

if HAVE_PYTHON
SUFFIXES += .h .hstamp

.h.hstamp:
	$(AM_V_GEN)$(run_python) $(srcdir)/build-aux/check-structs -I$(srcdir)/include $< && \
	touch $@

HSTAMP_FILES = \
	include/openflow/nicira-ext.hstamp \
	include/openflow/openflow-1.0.hstamp \
	include/openflow/openflow-1.1.hstamp \
	include/openflow/openflow-1.2.hstamp \
	include/openflow/openflow-1.3.hstamp \
	include/openflow/openflow-1.4.hstamp \
	include/openflow/openflow-1.5.hstamp \
	include/openflow/openflow-common.hstamp \
	include/openflow/openflow.hstamp
CLEANFILES += $(HSTAMP_FILES)
ALL_LOCAL += $(HSTAMP_FILES)
$(HSTAMP_FILES): build-aux/check-structs

include/openflow/openflow-1.0.hstamp: \
	include/openflow/openflow-common.h
include/openflow/openflow-1.1.hstamp: \
	include/openflow/openflow-common.h
include/openflow/openflow-1.2.hstamp: \
	include/openflow/openflow-common.h \
	include/openflow/openflow-1.1.h
include/openflow/openflow-1.3.hstamp: \
	include/openflow/openflow-common.h \
	include/openflow/openflow-1.1.h \
	include/openflow/openflow-1.2.h
include/openflow/openflow-1.4.hstamp: \
	include/openflow/openflow-1.4.h
include/openflow/openflow-1.5.hstamp: \
	include/openflow/openflow-1.5.h
include/openflow/nicira-ext.hstamp: \
	include/openflow/openflow.h \
	include/openflow/openflow-common.h \
	include/openflow/openflow-1.0.h \
	include/openflow/openflow-1.1.h \
	include/openflow/openflow-1.2.h \
	include/openflow/openflow-1.3.h
endif

EXTRA_DIST += build-aux/check-structs

