# MD

A playground to implement common market data and order management programs
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

I want to write a bunch of `transform` binaries that can process market data  
I would them pipe them together through various interfaces
like files, pipes, socket, share memory, etc  
I want to measure the latency impact of these pipelines
by comparing them to self-contained processes

## TODO

### io

- [x] file
  - [ ] file io benchmarks
- [ ] udp socket reader/writer
- [ ] multicast socket reader/writer
- [ ] pipes reader/writer
- [ ] shm reader/writer

### transform

- [x] stats
- [ ] filters
  - [ ] symbol filter
  - [ ] time filter
  - [ ] message type filter
  - [ ] common filtering api
- [x] to json
- [ ] order summary
- [ ] order blotter

### misc

- [ ] call shared library parser from python
- [ ] implement read ouch protocol
- [ ] synthetic data generator
- [x] change reinterpret_cast to bit_cast
- [ ] find a good arg parsing lib
