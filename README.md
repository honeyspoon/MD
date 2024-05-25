# MD

A playground to implement common market data programs
This is a toy project that is not intended to be used in production

## requirements

Look inside `build_tools.txt`

## build

`./build`  
`./build target`

## build & run

`./run target -- target_args`  
`./run ouch_stats  --  OUCHLMM2.incoming.packets`

## watch build & run

`./run --watch ouch_stats  --  OUCHLMM2.incoming.packets`

## goal

I want to write a bunch of `tranform` binaries that can process market data  
I would them pipe them together through vairous interfaces like files, pipes, socket, share memory, etc  
I want to measure the latency impact of these pipelines by comparing them to self-contained processes

## TODO

### io

- [x] file
  - [ ] file io benchmarks
- [ ] udp socket reader/writer
- [ ] multicast socket reader/writer
- [ ] pipes socket reader/writer
- [ ] shm socket reader/writer

### transform

- [x] stats
- [ ] filters
  - [ ] symbol filter
  - [ ] ts filter
- [ ] to json
- [ ] L1 BBO
- [ ] L2 book levels

### misc

- [ ] call shared library parser from python
- [ ] implement read ouch protocol
- [ ] synthetic data generator
- [ ] change reinterpret_cast to bit_cast
