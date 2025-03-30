# Makefile for cloning, building Yosys, and running synthesis and testbench

# Variables
YOSYS_REPO := https://github.com/YosysHQ/yosys.git
YOSYS_DIR := yosys

IVERILOG_REPO := https://github.com/steveicarus/iverilog.git
IVERILOG_DIR := iverilog

VERILOG_FILE ?= "your_design.v"
# Default target
all: .build-yosys

# Clone the Yosys repository
.clone-yosys:
	@if [ ! -d "$(YOSYS_DIR)" ]; then \
		git clone --recursive $(YOSYS_REPO); \
		touch .clone-yosys; \
		cd libs && unzip asap7-std-cells-lib-merged.lib.zip; \
	else \
		echo "Yosys repository already cloned."; \
	fi

# Build Yosys
.build-yosys: .clone-yosys
	@cd $(YOSYS_DIR) && make config-gcc && make -j$(nproc)
	touch .build-yosys

# Makefile for building Icarus Verilog (iverilog) from source

# Clone the Icarus Verilog repository
.clone-iverilog:
	@if [ ! -d "$(IVERILOG_DIR)" ]; then \
		git clone $(IVERILOG_REPO); \
		touch .clone-iverilog; \
	else \
		echo "Icarus Verilog repository already cloned."; \
	fi

# Build Icarus Verilog
.build-iverilog: .clone-iverilog
	@cd $(IVERILOG_DIR) && autoconf && ./configure --prefix=$(shell pwd)/iverilog-install && make -j$(nproc) && make install
	touch .build-iverilog

# Run synthesis with Yosys
synth: .build-yosys
	@$(YOSYS_DIR)/yosys -c synth.tcl 

# Run testbench with Verilator
tb: .build-iverilog
	./iverilog-install/bin/iverilog ${VERILOG_FILE} -o tb_run
	./tb_run

# Clean up
.PHONY: clean
clean:
	@rm -rf $(YOSYS_DIR) $(IVERILOG_DIR) iverilog-install tb_run *.vcd
	@rm -rf obj_dir
	@rm -f .build-yosys .clone-yosys .clone-iverilog .build-iverilog
	@echo "Cleaned up build files."

# Help target
.PHONY: help
help:
	@echo "Available targets:"
	@echo "  .clone-yosys     - Clone the Yosys repository."
	@echo "  .build-yosys     - Build Yosys from source."
	@echo "  synth           - Run synthesis with Yosys on input.v."
	@echo "  tb              - Run a iverilog-based testbench."
	@echo "  clean           - Clean up build files."
