myccd2cue
=========================

This is a crude port to Code::Blocks/MingW64. I basically hacked whatever is absolutely not necessary and was throwing an error.

I had to re-implement GNU's getline() quickly, so the implementation is probably not great but seems to work for the purpose.


Usage is different from cue2ccd because I removed argp library use (used to handle arguments parsing).


```
myccd2cue.exe --input file.ccd --output file.cue --image file.img
```

This was tested on a PS1 game, resulting CUE was then used to produce the CHD rom and tested on a PSX emulator running on a handheld.



