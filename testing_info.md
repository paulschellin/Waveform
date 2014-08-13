### Verify "Installation"
The Waveform library is ["header-only"](http://en.wikipedia.org/wiki/Header-only), so it does not require "installation" in the typical sense. But I've included the unit tests to help verify that your development environment is configured properly to use the Waveform library in your projects.

The test_src/Waveform_test.cpp file contains the beginnings of a testing suite for Waveform (using the Google Test framework).

The actual FFTW wrapper can be found in FftwTransform.hpp, which can be tested using the source file FftwTransform_test.cpp.

You must have the [Google Test framework](https://code.google.com/p/googletest/) installed in order to run these tests.

NOTE: Google recommends just placing the test framework directory within the directory of the project you wish to test -- so in this case, you would put "gtest-1.7.0" (or similar) into the Waveform/ directory.

IMPORTANT: Not all tests will pass necessarily, I've put explicit failures into the test files to remind myself and users that there are still features which I must implement.

There are a number of tests, each of which can be run by typing:

#### Test Waveform
This tests the basic functionality of the library.

```Shell
make clean Waveform
./test_bin/Waveform_test
```

#### Test IdentityTransform
Test the most basic transform -- the identity transform.
```Shell
make clean IdentityTransform
./test_bin/IdentityTransform_test
```

#### Test FftwTransform

NOTE: In order to run the FFTW tests, you must have the FFTW3 library installed. I highly recommend using your package manager (such as apt-get on Debian/Ubuntu, pacman on Arch, yum on Fedora, Homebrew on Mac OS X, etc.) to install it instead of installing it manually because your package manager will set up the linking and include directories automatically and appropriately.

```Shell
make clean FftwTransform
./test_bin/FftwTransform_test
```

