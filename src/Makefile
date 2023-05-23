CXX=msbuild
CXXFLAGS=/p:Configuration=Debug /p:Platform=x64

.PHONY: build
build:
	$(CXX) viewer\Viewer.vcxproj $(CXXFLAGS)

.PHONY: tests
tests:
	$(CXX) tests\VulkanTests.vcxproj $(CXXFLAGS)
	.\tests\x64\Debug\VulkanTests.exe

.PHONY: clean
clean:
	del /F viewer\Viewer.pdb viewer\Viewer.exe
	rd /s /q viewer\x64

.PHONY: install
install:
	powershell -Command "New-Item -ItemType SymbolicLink -Path \"$${HOME}\Desktop\Viewer.lnk\" -Target \"$(CURDIR)\viewer\Viewer.exe\""
