# Cache_simulator
This is a cache simulator with configurable parameters featuring:
- **Two-level cache architecture** (L1 and L2)
- **Victim block** implementation to handle conflict misses
- **LRU (Least Recently Used)** replacement policy for efficient cache management

The simulator integrates with **CACTI** to measure performance metrics such as access time, energy, and area.
The simulation outputs are stored in the outputs folder. These results correspond to various configurations tested with the simulator.

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

