# Cache_simulator
This is a cache simulator with configurable parameters featuring:
- **Two-level cache architecture** (L1 and L2)
- **Victim block** implementation to handle conflict misses
- **LRU (Least Recently Used)**,**PLRU ( Pseudo Least Recently Used)** ,**Round Robin**,**Random** replacement policies.

The simulator integrates with **CACTI** to measure performance metrics such as access time, energy, and area.
The simulation outputs are stored in the outputs folder. These results correspond to various configurations tested with the simulator using different replacement policies.

Run the following command to compile the simulator:
```bash
make raw
```
Execute the simulator with :
```bash
make run
```
To analyze the performance of the simulator, use:
```bash
make performance
```


