# Makefile for switch simulators
 
CC := gcc 
CXX := g++
RM = gio trash -f 
MKDIRS = mkdir -p
CLEAN_TEMPORARIES = $(RM) $(EXPONEDIR) $(EXPTWODIR)


TOP := $(shell pwd)
SRCDIR = $(TOP)/src
CXXSRCDIR = $(TOP)/cxx
BUILD = $(TOP)/build
EXPONEDIR = $(BUILD)/EXPONE
EXPTWODIR = $(BUILD)/EXPTWO

CFLAGS = -ansi -O3 -w 
CXXFLAGS = -std=c++11 -O3 -w 
LDFLAGS = -lm
INC := -I $(TOP)/include


SRCEXT := c
CXXSRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
CXXSOURCES := $(shell find $(CXXSRCDIR) -type f -name *.$(CXXSRCEXT))

OBJECTS_EXP_ONE := $(patsubst $(SRCDIR)/%,$(EXPONEDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CXXOBJECTS_EXP_ONE := $(patsubst $(CXXSRCDIR)/%,$(EXPONEDIR)/%,$(CXXSOURCES:.$(CXXSRCEXT)=.o))
OBJECTS_EXP_TWO := $(patsubst $(SRCDIR)/%,$(EXPTWODIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CXXOBJECTS_EXP_TWO := $(patsubst $(CXXSRCDIR)/%,$(EXPTWODIR)/%,$(CXXSOURCES:.$(CXXSRCEXT)=.o))


all: $(BUILD)/ss_experiment_one $(BUILD)/ss_experiment_two

$(BUILD)/ss_experiment_one: $(OBJECTS_EXP_ONE) $(CXXOBJECTS_EXP_ONE)
	@echo " Linking ..."
	$(CXX) $^ -o $@ $(LDFLAGS)
	@echo " Cleaning ..."
	$(CLEAN_TEMPORARIES)

$(BUILD)/ss_experiment_two: $(OBJECTS_EXP_TWO) $(CXXOBJECTS_EXP_TWO)
	@echo " Linking ..."
	$(CXX) $^ -o $@ $(LDFLAGS)
	@echo " Cleaning ..."
	$(CLEAN_TEMPORARIES)
	
$(EXPONEDIR):
	$(MKDIRS) $@

$(EXPTWODIR):
	$(MKDIRS) $@

$(EXPONEDIR)/%.o: $(CXXSRCDIR)/%.$(CXXSRCEXT) $(EXPONEDIR)
	$(CXX) $(CXXFLAGS) -imacros config_experiment_one.cfg $(INC) -c -o $@ $<

$(EXPONEDIR)/%.o: $(SRCDIR)/%.$(SRCEXT) $(EXPONEDIR)
	$(CC) $(CFLAGS) -imacros config_experiment_one.cfg $(INC) -c -o $@ $<

$(EXPTWODIR)/%.o: $(CXXSRCDIR)/%.$(CXXSRCEXT) $(EXPTWODIR)
	$(CXX) $(CXXFLAGS) -imacros config_experiment_two.cfg $(INC) -c -o $@ $<

$(EXPTWODIR)/%.o: $(SRCDIR)/%.$(SRCEXT) $(EXPTWODIR)
	$(CC) $(CFLAGS) -imacros config_experiment_two.cfg $(INC) -c -o $@ $<

experiment: $(BUILD)/ss_experiment_one $(BUILD)/ss_experiment_two
	echo " Performing Experiments ..."
	$(BUILD)/ss_experiment_one
	$(BUILD)/ss_experiment_two

clean:
	@echo " Cleaning ..."; 
	$(RM) $(BUILD)


.PHONY: clean clean-intermediate