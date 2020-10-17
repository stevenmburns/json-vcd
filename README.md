# To compile and run DPI testbench:

```bash
g++ -c -Ijson/include dpi_bench.cpp && vcs -sverilog dpi_bench.o json_testbench.v test_run_dir/json_vcd.JsonTester493020450/GCDInner.v
./simv
```
