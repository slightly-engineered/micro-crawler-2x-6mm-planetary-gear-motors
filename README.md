# Micro Crawler: 2 x 6 mm Planetary Gear Motors

A BEAM-style crawler driven by two 6 mm planetary gear motors: one front, one
back. Short full-power strokes alternate diagonally to crawl.

| Sketch | Board |
|---|---|
| `c3_crawler/` | ESP32-C3 Zero with two RZ7888 H-bridge motor drivers |

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
