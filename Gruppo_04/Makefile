MAKE = nmake

all: build_engine build_client

build_engine: 
	cd engine && $(MAKE) all && cd ..

build_client: build_engine
	cd client && $(MAKE) all && cd ..

package: build_client
	if exist dist\release rmdir /s /q dist\release
	if exist dist\assets rmdir /s /q dist\assets
	if exist dist\release.zip del /q dist\release.zip
	if not exist dist mkdir dist
	if not exist dist\release mkdir dist\release
	if not exist dist\assets mkdir dist\assets
	xcopy /E /I /Y assets dist\assets
	copy /Y client\bin\Release\client.exe dist\release
	copy /Y engine\bin\Release\engine.dll dist\release
	copy /Y client\obj\Release\main.obj dist\release\client.obj
	copy /Y engine\obj\Release\engine.obj dist\release\engine.obj
	echo @echo off > dist\release\run.bat
	echo set PATH=%%~dp0;%%PATH%% >> dist\release\run.bat
	echo "%%~dp0client.exe" %%* >> dist\release\run.bat
	cd dist && tar -a -c -f release.zip release assets && cd ..

clean: clean_engine clean_client clean_package

clean_package:
	if exist dist rmdir /s /q dist

clean_engine: 
	cd engine && $(MAKE) clean && cd ..

clean_client: 
	cd client && $(MAKE) clean && cd ..

test:
	cd engine && $(MAKE) test && cd ..