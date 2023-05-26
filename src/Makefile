CXX=msbuild
CXXFLAGS=/p:Configuration=Debug /p:Platform=x64

all: unpack_gtest_libs unpack_glfw_libs tests install dvi

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
	rd /s /q tests\x64

.PHONY: uninstall
uninstall:
	rd /s /q viewer\build

.PHONY: dvi
dvi:
	docsify serve .\..\materials\docs\

.PHONY: dist
dist:
	powershell Compress-Archive $(CURDIR)\viewer\build Viewer.zip

.PHONY: unpack_glfw_libs
unpack_glfw_libs:
	powershell Expand-Archive -Path $(CURDIR)\libraries\glmandglfwlibs.zip -DestinationPath $(CURDIR)\libraries\

.PHONY: unpack_gtest_libs
unpack_gtest_libs:
	powershell Expand-Archive -Path $(CURDIR)\tests\packages.zip -DestinationPath $(CURDIR)\tests\
