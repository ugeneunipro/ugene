## Build

For Unix systems:
```bash
qmake
make
```
## Running Tests
A single test can be run just using environment variable 'HI_GUI_TEST'.
For Unix systems:
```bash
export LD_LIBRARY_PATH=../../_build/release/:../../_build/debug/
export HI_GUI_TEST=Calc_base_tests:Test1
./calculator
```
or for Windows systems:
```bat
set HI_GUI_TEST=Calc_base_tests:Test1
calculator.exe
```
