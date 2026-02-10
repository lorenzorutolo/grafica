MAKE = make

PACKAGE_DIR := dist
PACKAGE_NAME := release
PACKAGE_STAGE := $(PACKAGE_DIR)/$(PACKAGE_NAME)
PACKAGE_ASSETS := $(PACKAGE_DIR)/assets
PACKAGE_ZIP := $(PACKAGE_DIR)/$(PACKAGE_NAME).zip
ASSETS_SRC := assets

all: build_engine build_client

build_engine: 
	$(MAKE) -C engine all

build_client: build_engine
	$(MAKE) -C client all

package: $(PACKAGE_ZIP)

$(PACKAGE_ZIP): build_client
	rm -rf $(PACKAGE_STAGE) $(PACKAGE_ASSETS) $(PACKAGE_ZIP)
	mkdir -p $(PACKAGE_STAGE)
	mkdir -p $(PACKAGE_ASSETS)
	cp -r $(ASSETS_SRC)/* $(PACKAGE_ASSETS)/
	cp client/bin/Release/client $(PACKAGE_STAGE)/
	cp engine/bin/Release/libengine.so $(PACKAGE_STAGE)/
	cp client/obj/Release/main.o $(PACKAGE_STAGE)/client.o
	cp engine/obj/Release/engine.o $(PACKAGE_STAGE)/engine.o
	echo '#!/usr/bin/env bash' > $(PACKAGE_STAGE)/run.sh
	echo 'set -euo pipefail' >> $(PACKAGE_STAGE)/run.sh
	echo 'SCRIPT_DIR="$$(cd "$$(dirname "$$0")" && pwd)"' >> $(PACKAGE_STAGE)/run.sh
	echo 'export LD_LIBRARY_PATH="$$SCRIPT_DIR:$${LD_LIBRARY_PATH:-}"' >> $(PACKAGE_STAGE)/run.sh
	echo '"$$SCRIPT_DIR/client" "$$@"' >> $(PACKAGE_STAGE)/run.sh
	chmod +x $(PACKAGE_STAGE)/run.sh
	cd $(PACKAGE_DIR) && zip -r $(PACKAGE_NAME).zip $(PACKAGE_NAME) assets

clean: clean_engine clean_client clean_package

clean_package:
	rm -rf $(PACKAGE_DIR)

clean_engine: 
	$(MAKE) -C engine clean

clean_client: 
	$(MAKE) -C client clean

test:
	$(MAKE) -C engine test

.PHONY: clean_engine clean_client test package clean_package
