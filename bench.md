## reading files

```
./build && hyperfine  --prepare 'sync && sudo purge' 'out/ouch_stats OUCHLMM2.incoming.packets'
```

seems like fstream is the fastest when I purge the cache

