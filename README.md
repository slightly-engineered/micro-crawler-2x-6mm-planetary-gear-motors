# Micro Crawler: 2 x 6 mm Planetary Gear Motors

A BEAM-style crawler driven by two 6 mm planetary gear motors: one front, one
back. Short full-power strokes alternate diagonally to crawl.

| Folder | Contents |
|---|---|
| `c3_crawler/` | Arduino sketch: ESP32-C3 Zero with two RZ7888 H-bridge motor drivers |
| `stl/` | 3D-printable parts and fit-check reference models |

## Setup

Run once from this folder in PowerShell to install Arduino IDE and the ESP32
board package:

```powershell
powershell -ExecutionPolicy Bypass -File .\setup.ps1             # asks before each install
powershell -ExecutionPolicy Bypass -File .\setup.ps1 -CheckOnly  # only reports what is missing
```

## 3D-printed parts

Print these from `stl/`:

| File | Qty | Notes |
|---|---|---|
| `micro crawler body.stl` | 1 | Main chassis |
| `6mm planetary motor horn v2.stl` | 2 | Motor horn / leg, current version |
| `6mm planetary motor horn.stl` | - | Original horn, superseded by v2 |

Reference models (for fit checks in a slicer or CAD, not for printing):

| File | Represents |
|---|---|
| `front motor.stl`, `rear motor.stl` | 6 mm planetary gear motors in their mounted positions |
| `Arduino ESP32-C3.stl` | ESP32-C3 board |
| `DRV8833.stl` | DRV8833 motor driver board |
| `battery dummy.stl` | 160 mAh LiPo cell |

The meshes are decimated for size; the Blender source stays off the repo.

## Parts

| Part | Note | Link |
|---|---|---|
| 6 mm planetary gear motor (x2) | 46 RPM option | [AliExpress](https://www.aliexpress.us/item/3256812043824069.html) |
| ESP32-C3 Super Mini | | [AliExpress](https://www.aliexpress.us/item/3256807292829704.html) |
| DRV8833 driver board | | [AliExpress](https://www.aliexpress.us/item/3256809648666681.html) |
| LiPo battery | "2pcs 160mAh" option | [AliExpress](https://www.aliexpress.us/item/3256812564147983.html) |

## Gait

1. Front motor, direction 1 (front-right foot down)
2. Back motor, direction 1 (back-left foot kicks)
3. Front motor, direction 2 (front-left foot down)
4. Back motor, direction 2 (back-right foot kicks)

Each stroke runs at full duty for a short time, then pauses; consecutive
strokes overlap slightly. The timing is non-blocking.

## Wiring

Front motor driver: IN A -> GPIO 4, IN B -> GPIO 5. See the top of
`c3_crawler.ino` for the back motor pins. The ESP32-C3 strapping pins
(GPIO 2, 8, 9) are deliberately unused.

Requires ESP32 Arduino core 3.x (`ledcAttach` / `ledcWrite` by pin).

## License

MIT. See [LICENSE](LICENSE).