.SECONDEXPANSION:
ROOT=../../..
OBJDIR=.obj
MODULES=$(notdir $(wildcard $(ROOT)/examples/*))
CXXFLAGS += -Wno-narrowing -fpermissive -O2 -fPIC -D_DEFAULT_SOURCE
LDFLAGS  += -shared -fPIC -lm -ldl -lrt -lpthread -Wl,-rpath,./ -Wl,-rpath-link,$(MPLCLIBS)
CXX ?=	g++
CC ?=	gcc
INCLUDES =	-I$(ROOT)/include \
			-I$(ROOT)/include/core \
			-I$(ROOT)/lib \
			-I$(ROOT)/lib/opcua

DEFINES= \
	-DLINUX \
	-D_GNUC_ \
	-DTARGET_PLATFORM_NAME=\"$(TARGET)\" \
	-D'BOOST_USER_CONFIG=<mplc/detail/mplc_boost_config.hpp>' \

all: $$(MODULES)
	@echo Done

clear:
	@echo Clearing
	@rm -rf $(OUT) $(OBJDIR)

echo_target_%: TEXT=\$(subst ;,\n,\
"--------- $* ------------;\
 | SRCDIR= $(SRCDIR);\
 | SRC= $(SRC);\
 | OBJ= $(OBJ);\
 | CXXFLAGS=$(CXXFLAGS);\
 | LDFLAGS=$(LDFLAGS);\
 | DEP=$(DEPs);\
 | DEFINES=$(DEFINES);\
-----------------------------")

echo_target_%:
	@echo $(TEXT)
	@if [ -z "$(SRC)" ]; then echo "Error: No files to compile in src directory"; exit 1; fi


define CreateNewTarget
# $1 : NAME
$1_OBJS=$$(OBJDIR)/$1/%.o
$1=$1.so
$1: $1.so
$$($1): SRCDIR=$$(ROOT)/examples/$1
$$($1): EXCLUDE=%dllmain.cpp
$$($1): SRC=$$(shell find $$(ROOT)/examples/$1 -type f -name '*.cpp') $$(shell find $$(ROOT)/examples/$1 -type f -name '*.c')
$$($1): _SRC_=$$(filter-out $$(EXCLUDE),$$(SRC))
$$($1): OBJ=$$(addsuffix .o,$$(basename $$(_SRC_:$$(ROOT)/examples/$1/%=$$(OBJDIR)/$1/%)))
$$($1): _OBJ_=$$(OBJ:$$(OBJDIR)/$1/%=%) # For echo_target
$$($1): echo_target_$1 $$$$(OBJ)
	@echo "Linking $$@"
	$$(CXX) $$(CXXFLAGS) $$(OBJ) -Wl,-Map,$1.map $$(LDFLAGS) $$(DEPs) -o $$@

$$($1_OBJS): $$$$(SRCDIR)/%.cpp | echo_target_$1
	@mkdir -p `dirname $$@`
	$$(CXX) -I$$(SRCDIR) $$(DEFINES) $$(INCLUDES) $$(CXXFLAGS) $$(FLAGS) -c -o $$@ $$<

$$($1_OBJS): $$$$(SRCDIR)/%.c | echo_target_$1
	@mkdir -p `dirname $$@`
	$$(CC) -I$$(SRCDIR) $$(DEFINES) $$(INCLUDES) $$(CFLAGS) $$(FLAGS) -c -o $$@ $$<

endef

$(foreach target,$(MODULES),$(eval $(call CreateNewTarget,$(target))))

