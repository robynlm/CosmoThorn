# -*-Makefile-*-

KRANC = /Users/rlm36AA/codes/ET/ET_2023_11/Cactus/repos/Kranc/Bin/kranc

.PHONY: CosmoLapse
CosmoLapse:
	@echo "Creating the CosmoLapse thorn on $(shell date)"
	@rm -f CosmoLapse.out
	@rm -rf CosmoLapse
	@$(KRANC) CosmoLapse.m | tee CosmoLapse.out
	@echo "Done :D"
