# 8080Emu
i8080 state/cycle accurate emulator which tries to mimick a possible hardware implementation (not the actual i8080 chipset).

Still a work in progress because it fails 3 out of the 25 tests of the [8080 exerciser](https://web.archive.org/web/20151006085348/http://www.idb.me.uk/sunhillow/8080.html) (aluop nn, aluop r/m, daa/cma/cmc/stc).
The rest of the tests produce the correct CRC32 values.

**WARNING**: This is not an ISA level emulator! As a result, it's really slow! You've been warned :)

The microcoded ROM will be used in [DLS](https://makingartstudios.itch.io/dls) to implement the instruction decoder/sequencer (if I find some time to actually finish it).

### Output
![8080 Exerciser output](https://cdn.rawgit.com/jdryg/8080Emu/master/img/8080_exerciser_output.png)

