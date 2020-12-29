# map-env

connects to Arduino through Serial, reads the values GY-VL53L0XV2 module manages to scan by being rotated 180 degrees counterclockwise, and graphically displays the gathered results

## produced results

the script manages to produce the following graph:

<img src="../../docs/myplot_2.png" alt="myplot_2" style="zoom:75%;" />

and here's everything explained:

<img src="../../docs/myplot_2_explained.png" alt="myplot_2_explained" style="zoom:75%;" />

## dependencies

for Debian based distros, the following packages are required:

```
python3-matplotlib
python3-pip
```

and then through pip3 install this library:

```
pyserial
```

## running

```bash
python3.8 map-env.py
```
