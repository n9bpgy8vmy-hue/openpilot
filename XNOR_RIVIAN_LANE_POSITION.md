# Rivian lane-position assistance for XNOR

This branch ports the bounded Rivian lane-position and curve-adjustment controller
to XNOR's `rx-dev-src` architecture. It does not include RivianPilot's MADS,
post-turn, navigation, or vision-BSM changes.

## Initial validation

1. Install the `rx-laneposition` branch.
2. Leave **Go Live** disabled and enable **Observe Lane Position** plus
   **Diagnostic Logging**.
3. Confirm normal XNOR startup and collect a short drive before enabling control.
4. Enable **Go Live** only for a controlled road test with the driver ready to
   steer at all times.

The runtime offset is Rivian-only, bounded to 15 inches, canceled immediately by
driver steering or a blinker, and rejected by modeld when its heartbeat is stale,
invalid, or outside the bound. A rejected or failed custom input restores the
original XNOR camera offset/path.

## Local focused tests

```sh
uv run --frozen --extra testing pytest -q -n 0 \
  --confcutdir=openpilot/sunnypilot \
  openpilot/sunnypilot/rivian_lane_position/tests/test_lane_position_controller.py \
  openpilot/sunnypilot/modeld_v2/tests/test_camera_offset_helper.py \
  openpilot/sunnypilot/sunnylink/tests/test_compile_settings_ui.py \
  openpilot/sunnypilot/sunnylink/tests/test_settings_changes.py
```
