# MD

A playground to implement common market protocols
This is a toy project that is not intended to be used in production

## requirements

Look inside `build_tools.txt`  
probably the latest clang and cmake

## usage

### build

`./build`  
`./build target`

### build & run

`./run target -- target_args`  
`./run ouch_stats  --  OUCHLMM2.incoming.packets`

### watch build & run

`./run --watch ouch_stats  --  OUCHLMM2.incoming.packets`

### test

`./test`

## goal

I want to write a bunch of `transform` binaries that can process common market protocols
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
- [x] pipes reader/writer
- [ ] shm reader/writer

### transform

- [x] stats
- [ ] filters
  - [x] symbol filter
  - [ ] time filter
  - [ ] message type filter
  - [ ] common filtering api
- [x] to json
- [ ] order summary
- [ ] order blotter

### misc

- [ ] detect if in piple
  - [ ] isatty
  - [ ] change default args for input base on that
- [ ] call shared library parser from python
- [ ] implement read ouch protocol
- [ ] synthetic data generator
- [x] change reinterpret_cast to bit_cast
- [x] find a good arg parsing lib

## weird things

### variant readers/writer

My use of this is purely ideological  
I could have opted for a reader base class and use inheritance  
I also could have done the same with CRTP allowing me to not have a vtable indirection

The main idea here is that I didn't want to use reference semantics and variants allow me to do that  
But value semantics don't really make sense for reader or writer objects since they own ressources  
But I can use variant with move semantics so it works

### pipelines

Making pipelines is probably the worst idea in low latency applications but might be good for throughput  
I don't recommend doing this but it's a fun idea to test  
Once again it's purely ideological since I like the unix philosophy
