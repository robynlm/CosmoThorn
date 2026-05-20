# -*-Makefile-*-

KRANC = /Users/rlm36AA/codes/ET/ET_2023_11/Cactus/repos/Kranc/Bin/kranc

.PHONY: CosmoLapse
CosmoLapse:
	@echo "Creating the CosmoLapse thorn on $(shell date)"
	@rm -f CosmoLapse.out
	@rm -rf CosmoLapse
	@$(KRANC) CosmoLapse.m | tee CosmoLapse.out
	@echo "Done :D"

.PHONY: CosmoLapse_CTD
CosmoLapse_CTD:
	@echo "Creating the CosmoLapse_CTD thorn on $(shell date)"
	@rm -f CosmoLapse_CTD.out
	@rm -rf CosmoLapse_CTD
	@$(KRANC) CosmoLapse_CTD.m | tee CosmoLapse_CTD.out
	@echo "Done :D"