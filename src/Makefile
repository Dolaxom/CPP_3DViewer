CXX=msbuild
CXXFLAGS=/p:Configuration=Debug /p:Platform=x64

all: tests install dvi

.PHONY: build
build:
	$(CXX) viewer\Viewer.vcxproj $(CXXFLAGS)

.PHONY: install
install: build
	mkdir viewer\build
	copy viewer\Viewer.exe viewer\build
	copy viewer\launch_parameters.conf viewer\build
	copy viewer\imgui.ini viewer\build
	copy viewer\simple_shader.frag.spv viewer\build
	copy viewer\simple_shader.vert.spv viewer\build
	del /F viewer\Viewer.exe

.PHONY: tests
tests:
	$(CXX) tests\VulkanTests.vcxproj $(CXXFLAGS)
	.\tests\x64\Debug\VulkanTests.exe

.PHONY: clean
clean:
	del /F viewer\Viewer.pdb
	rd /s /q viewer\x64
	rd /s /q viewer\build

.PHONY: uninstall
uninstall:
	rd /s /q viewer\build

.PHONY: dvi
dvi:
	docsify serve .\..\materials\docs\

.PHONY: dist
dist:
	powershell Compress-Archive $(CURDIR)\viewer\build Viewer.zip