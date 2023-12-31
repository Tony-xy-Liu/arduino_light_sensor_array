import os, sys
from pathlib import Path
import serial
from serial import SerialException
import time
import json
from datetime import datetime as dt
# import numpy as np

class StdTime:
    FORMAT = '%Y-%m-%d_%H-%M-%S'

    @classmethod
    def Timestamp(cls, timestamp: dt|None = None):
        ts = dt.now() if timestamp is None else timestamp
        return f"{ts.strftime(StdTime.FORMAT)}"
    
    @classmethod
    def Parse(cls, timestamp: str|int):
        if isinstance(timestamp, str):
            return dt.strptime(timestamp, StdTime.FORMAT)
        else:
            return dt.fromtimestamp(timestamp/1000)
    
    @classmethod
    def CurrentTimeMillis(cls):
        return round(time.time() * 1000)

def Monitor(matrix: list[list], minv: float, meanv: float, maxv: float):    
    def _to_greyscale(val):
        GREYSCALE = [
            "  ",
            "░░",
            "▒▒",
            "▓▓",
            "██",
        ]
        _min, _max = minv, maxv
        
        if _max-_min == 0:
            scaled = 0
        else:
            scaled = (val-_min)/(_max-_min)

        i = int(scaled * len(GREYSCALE) + 0.5)
        i = min(i, len(GREYSCALE)-1)
        i = max(i, 0)
        return GREYSCALE[i]
    
    def _pad(v: float):
        str_val = f"{v:0.2f}"
        pad = "0"*(6-len(str_val))
        return pad+str_val
    
    # matrix = matrix.copy()
    # matrix.reverse()
    for col in matrix:
        # str_col = [f"{int(round(v*10)):03}" for v in col]
        str_col = [_to_greyscale(v) for v in col]
        # print(",".join(str_col))
        print(",".join(str_col) + "    " + " ".join(_pad(v) for v in col))
    # top = 
    print(f"min:{_pad(minv)} | av:{_pad(meanv)} | max:{_pad(maxv)}")

def Run(out_path: Path, usb_port: str, baud_rate: int):
    if not out_path.exists():
        os.makedirs(out_path)
    run_name = f"{StdTime.Timestamp()}"

    arduino = None
    while True:
        try:
            arduino = serial.Serial(port=usb_port, baudrate=baud_rate, timeout=.1)
            break
        except SerialException:
            time.sleep(0.1)
    
    log_file = open(out_path.joinpath(f"{run_name}.csv"), "w")
    header: list[str] = []
    header += "elapsed_time, frame".split(", ")
    for row in range(4):
        for col in range(6):
            header.append(f"{row+1}_{col+1}")
    log_file.write(",".join(header)+"\n")

    # https://stackoverflow.com/questions/8290397/how-to-split-an-iterable-in-constant-size-chunks
    def _batch(iterable, n):
        l = len(iterable)
        for ndx in range(0, l, n):
            yield iterable[ndx:min(ndx + n, l)]
            
    def _to_matrix(vals: list[list]):
        HEIGHT = 6
        cols = [col for col in _batch(vals, HEIGHT)]
        # return np.array(cols)
        return cols

    last = 0
    SAVE_PERIOD = 1000
    buffer = []
    started = False
    while True:
        now = StdTime.CurrentTimeMillis()
        delta_t = now-last
        
        x = arduino.readline().decode(encoding="utf-8", errors="ignore")
        if len(x) == 0: continue
        entry: list|None = None
        try:
            entry = json.loads(x)
            assert entry is not None
            elapsed_time, frame = entry[:2]
            vals = entry[2:]
            if not started and elapsed_time != 0: continue
            started = True
            matrix = _to_matrix(vals)
            buffer.append([elapsed_time, frame]+[v for g in matrix for v in g])
        except json.JSONDecodeError:
            continue
        
        Monitor(matrix, min(vals), sum(vals)/len(vals), max(vals))

        if delta_t >= SAVE_PERIOD:
            log_file.writelines([
                ",".join([f"{v}" for v in entry])+"\n"
            for entry in buffer])
            buffer.clear()
            log_file.flush()
            last = now


    # for i in range(100):
    #     x = arduino.readline()
    #     print(x.decode(encoding="utf-8", errors="ignore"))
