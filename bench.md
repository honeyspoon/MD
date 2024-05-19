## reading files

```
make && hyperfine  --prepare 'sync && sudo purge' 'bin/main OUCHLMM2.incoming.packets'
```

seems like fstream is the fastest when I purge the cache