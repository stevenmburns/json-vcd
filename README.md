# To compile and run DPI testbench:

```bash
g++ --std=c++17 -c -Ijson/include dpi_bench.cpp
```

```bash
vcs -sverilog dpi_bench.o json_testbench.v test_run_dir/json_vcd.JsonTester493020450/GCDInner.v
```

```bash
./simv
```

# To compile and run the testbench client and server version
```bash
vcs -sverilog dpi_bench.o json_testbench_client.v json_testbench_server.v test_run_dir/json_vcd.JsonTester493020450/GCDInner.v
```

# And the Chisel version
```bash
sbt 'testOnly *.TopTest'
```

