# Plots
Plots can be found in /eval/plots_and_timings.
![alt text](https://github.com/ruckdack/parallel-pipeline/blob/d3acc7f322c301eb299fab4fb812825ee405170d/eval/plots_and_timings/plots_as_images/0001.jpg)
![alt text](https://github.com/ruckdack/parallel-pipeline/blob/d3acc7f322c301eb299fab4fb812825ee405170d/eval/plots_and_timings/plots_as_images/0002.jpg)
![alt text](https://github.com/ruckdack/parallel-pipeline/blob/d3acc7f322c301eb299fab4fb812825ee405170d/eval/plots_and_timings/plots_as_images/0003.jpg)
![alt text](https://github.com/ruckdack/parallel-pipeline/blob/d3acc7f322c301eb299fab4fb812825ee405170d/eval/plots_and_timings/plots_as_images/0004.jpg)

# How to execute

## Data generation
Generate data using 
```
./generate_data.sh
```
from /data.

## TBB
TBB needs to be cloned in /tbb.

## Tests
Tests are in /tests, the corresponding binary is ./tests.

## Timing
Call
```
./naive_timing && ./static_parallel_timing && ./dynamic_parallel_timing && ./tbb_timing
```
to generate timings and plot them via
```
python3 ../eval/plots.py
```
from the build directory.
