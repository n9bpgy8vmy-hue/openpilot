from types import SimpleNamespace
from unittest.mock import MagicMock

from opendbc.car import structs

from openpilot.sunnypilot.rivian_lane_position.lane_position_controller import LanePositionController


def params(overrides=None):
  values = {
    "RivianPilotLanePositionObserve": True,
    "RivianPilotLanePositionGoLive": True,
    "RivianPilotFeatureLogging": False,
    "RivianPilotCurveOffset": True,
    "RivianPilotLanePositionRelaxed": False,
    "RivianPilotNudgeOffset": True,
    "RivianPilotCurveOffsetInches": 3,
    "RivianPilotCurveThreshold": 35,
    "RivianPilotLanePositionPreference": 0,
    "RivianPilotCenterCorrectionInches": 5,
    "RivianPilotAdaptiveCenterCorrection": False,
    "RivianPilotCenterMinimumSpeed": 15,
    "RivianPilotCenterFullSpeed": 30,
    "RivianPilotCenterHighwaySpeed": 55,
    "RivianPilotHighwayCenterCorrectionInches": 3,
    "RivianPilotLanePositionBiasInches": 3,
    "RivianPilotNudgeOffsetInches": 3,
    "RivianPilotNudgeTorqueThresholdTenths": 7,
    "RivianPilotNudgeHoldSeconds": 10,
    "RivianPilotNudgeTimerDisplay": True,
    "RivianPilotActiveOffsetDisplay": True,
    "RivianPilotLaneCorrectionAlert": False,
    "RivianPilotVehicleWidthInches": 82,
    "RivianPilotBoundaryBufferInches": 5,
    "RivianPilotPoorRoadOffsetInches": 2,
    "CameraOffset": 0.0,
  }
  values.update(overrides or {})
  p = MagicMock()
  p.get_bool.side_effect = lambda key: bool(values[key])
  p.get.side_effect = lambda key, **kwargs: values[key]
  return p


def car_state():
  return SimpleNamespace(
    gearShifter=structs.CarState.GearShifter.drive,
    vEgo=30.0,
    steeringTorque=0.0,
    steeringPressed=False,
    leftBlinker=False,
    rightBlinker=False,
  )


def model(probability=0.9, lane_half_width=1.8, path_y=0.0, far_lane_half_width=None, near_lane_center=0.0,
          predicted_lat_accel=0.0, predicted_speed=30.0):
  x = [0.0, 10.0, 20.0, 60.0]
  half_widths = [lane_half_width] * 3 + [far_lane_half_width if far_lane_half_width is not None else lane_half_width]
  times = [0.0, 0.5, 1.0, 2.0]
  yaw_rate = predicted_lat_accel / predicted_speed if predicted_speed > 0.0 else 0.0
  return SimpleNamespace(
    laneLineProbs=[0.0, probability, probability, 0.0],
    laneLines=[
      SimpleNamespace(x=x, y=[0.0] * 4),
      SimpleNamespace(x=x, y=[-half_widths[0] + near_lane_center] + [-width for width in half_widths[1:]]),
      SimpleNamespace(x=x, y=[half_widths[0] + near_lane_center] + half_widths[1:]),
    ],
    position=SimpleNamespace(x=x, y=[path_y] * 4, t=times),
    orientationRate=SimpleNamespace(z=[yaw_rate] * 4),
    velocity=SimpleNamespace(x=[predicted_speed] * 4),
  )


def establish_curve(feature, cs=None, road_model=None, curvature=0.002, start=1.0):
  cs = cs or car_state()
  road_model = road_model or model()
  initial_path_y = float(road_model.position.y[0])
  controls = SimpleNamespace(desiredCurvature=curvature)
  for now in (start, start + 0.2, start + 0.4, start + 0.9, start + 1.4):
    if feature.lane_position_preference:
      road_model.position.y = [initial_path_y - feature.automatic_output] * len(road_model.position.y)
    feature.update(cs, True, road_model, controls, now=now)
  return controls


def establish_straight(feature, road_model=None, start=1.0):
  cs = car_state()
  road_model = road_model or model()
  initial_path_y = float(road_model.position.y[0])
  controls = SimpleNamespace(desiredCurvature=0.0)
  for i in range(12):
    road_model.position.y = [initial_path_y - feature.automatic_output] * len(road_model.position.y)
    feature.update(cs, True, road_model, controls, now=start + i * 0.2)
  return controls


def test_curve_offset_is_bounded_and_away_from_inside():
  p = params()
  feature = LanePositionController(p)
  establish_curve(feature, curvature=0.002)
  assert feature.last_output > 0.0
  assert abs(feature.last_output) <= 3 * 0.0254

  feature = LanePositionController(p)
  establish_curve(feature, curvature=-0.002)
  assert feature.last_output < 0.0
  assert abs(feature.last_output) <= 3 * 0.0254


def test_lane_correction_alert_publishes_once_per_automatic_curve_episode():
  p = params()
  memory = MagicMock()
  original_get_bool = p.get_bool.side_effect
  p.get_bool.side_effect = lambda key: True if key == "RivianPilotLaneCorrectionAlert" else original_get_bool(key)
  feature = LanePositionController(p, memory)
  establish_curve(feature, curvature=0.002)

  direction_calls = [call for call in memory.put.call_args_list if call.args and call.args[0] == "RivianPilotLaneCorrectionAlertDirection"]
  assert len(direction_calls) == 1
  assert direction_calls[0].args[1] == "left"
  inch_calls = [call for call in memory.put.call_args_list if call.args and call.args[0] == "RivianPilotLaneCorrectionAlertInches"]
  assert inch_calls[-1].args[1] == 3
  assert not any(call.args and call.args[0].startswith("RivianPilotLaneCorrectionAlert") for call in p.put.call_args_list)

def test_curve_activates_at_configured_threshold():
  p = params()
  values = {
    "RivianPilotLanePositionObserve": True, "RivianPilotLanePositionGoLive": True,
    "RivianPilotFeatureLogging": False, "RivianPilotCurveOffset": True,
    "RivianPilotLanePositionRelaxed": False, "RivianPilotNudgeOffset": True,
  }
  p.get_bool.side_effect = lambda key: values[key]
  original_get = p.get.side_effect
  p.get.side_effect = lambda key, **kwargs: 30 if key == "RivianPilotCurveThreshold" else original_get(key, **kwargs)
  feature = LanePositionController(p)

  # 0.001 curvature at 30 m/s is 0.9 m/s^2, exactly 30% of the
  # controller's 3.0 m/s^2 reference lateral acceleration.
  establish_curve(feature, curvature=0.001)

  assert feature.curve_threshold_pct == 30
  assert feature.curve_active
  assert feature.last_output > 0.0


def test_curve_does_not_activate_below_configured_threshold():
  p = params()
  values = {
    "RivianPilotLanePositionObserve": True, "RivianPilotLanePositionGoLive": True,
    "RivianPilotFeatureLogging": False, "RivianPilotCurveOffset": True,
    "RivianPilotLanePositionRelaxed": False, "RivianPilotNudgeOffset": True,
  }
  p.get_bool.side_effect = lambda key: values[key]
  original_get = p.get.side_effect
  p.get.side_effect = lambda key, **kwargs: 30 if key == "RivianPilotCurveThreshold" else original_get(key, **kwargs)
  feature = LanePositionController(p)

  establish_curve(feature, curvature=0.00099)

  assert not feature.curve_active
  assert feature.last_output == 0.0


def test_dynamic_offset_params_are_written_as_runtime_float_types():
  p = params()
  LanePositionController(p)

  offset_write = next(call for call in p.put.call_args_list if call.args[0] == "RivianPilotDynamicCameraOffset")
  heartbeat_write = next(call for call in p.put.call_args_list if call.args[0] == "RivianPilotDynamicCameraOffsetUpdated")
  assert isinstance(offset_write.args[1], float)
  assert isinstance(heartbeat_write.args[1], float)


def test_low_confidence_blocks_automatic_curve_offset():
  p = params()
  feature = LanePositionController(p)
  establish_curve(feature, road_model=model(probability=0.2))
  assert not feature.faulted
  assert feature.last_output == 0.0


def test_relaxed_geometry_is_explicit_and_bounded_to_two_inches():
  p = params()
  original_get_bool = p.get_bool.side_effect
  p.get_bool.side_effect = lambda key: True if key == "RivianPilotLanePositionRelaxed" else original_get_bool(key)
  feature = LanePositionController(p)
  establish_curve(feature, road_model=model(probability=0.2))
  assert feature.last_output == 2 * 0.0254


def test_driver_input_immediately_publishes_zero_then_latches_nudge():
  p = params()
  feature = LanePositionController(p)
  cs = car_state()
  cs.steeringPressed = True
  cs.steeringTorque = 2.0
  feature.update(cs, True, model(), SimpleNamespace(desiredCurvature=0.0), now=1.0)
  assert feature.last_output == 0.0
  cs.steeringPressed = False
  cs.steeringTorque = 0.0
  feature.update(cs, True, model(), SimpleNamespace(desiredCurvature=0.0), now=1.2)
  assert feature.last_output > 0.0


def test_manual_nudge_is_authoritative_without_lane_geometry():
  p = params()
  feature = LanePositionController(p)
  cs = car_state()
  feature.nudge_direction = 1
  feature.nudge_until = 20.0
  feature.update(cs, True, model(probability=0.0), SimpleNamespace(desiredCurvature=0.0), now=1.0)
  assert feature.last_output == 3 * 0.0254

  feature = LanePositionController(p)
  feature.nudge_direction = -1
  feature.nudge_until = 20.0
  feature.update(cs, True, model(probability=0.0), SimpleNamespace(desiredCurvature=0.0), now=1.2)
  assert feature.last_output == -3 * 0.0254


def test_automatic_guard_uses_only_movement_side_clearance():
  p = params()
  cs = car_state()
  # Positive curvature uses positive curve-output polarity. Clearance must be
  # checked on that same physical movement side.
  feature = LanePositionController(p)
  establish_curve(feature, cs, model(lane_half_width=1.5, path_y=-0.3))
  assert feature.last_output == 3 * 0.0254

  # Future planned-path placement is diagnostic rather than a proxy for the
  # truck's current physical position.
  feature = LanePositionController(p)
  establish_curve(feature, cs, model(lane_half_width=1.5, path_y=0.3))
  assert feature.last_output == 3 * 0.0254


def test_curve_avoidance_has_priority_over_lane_position_bias():
  # After the Rivian-specific polarity correction, a positive-curvature curve
  # requires at least three inches of positive curve output. Lane preference
  # must not cancel that curve-specific requirement.
  left_bias = LanePositionController(params({
    "RivianPilotLanePositionPreference": 2,
    "RivianPilotLanePositionBiasInches": 5,
  }))
  establish_curve(left_bias, curvature=0.002)
  assert left_bias.last_output >= 3 * 0.0254

  right_bias = LanePositionController(params({
    "RivianPilotLanePositionPreference": 3,
    "RivianPilotLanePositionBiasInches": 5,
  }))
  controls = establish_curve(right_bias, curvature=0.002)
  feedback_model = model(path_y=-right_bias.automatic_output)
  for now in (2.6, 2.8, 3.0):
    feedback_model.position.y = [-right_bias.automatic_output] * len(feedback_model.position.y)
    right_bias.update(car_state(), True, feedback_model, controls, now=now)
  assert right_bias.last_output >= 3 * 0.0254


def test_default_follows_model_on_straight_road():
  feature = LanePositionController(params({"RivianPilotLanePositionPreference": 0}))
  establish_straight(feature, model(path_y=0.3))
  assert feature.last_output == 0.0


def test_center_corrects_model_bias_on_straight_road():
  feature = LanePositionController(params({
    "RivianPilotLanePositionPreference": 1,
    "RivianPilotCenterCorrectionInches": 4,
  }))
  road_model = model(path_y=10 * 0.0254)
  controls = SimpleNamespace(desiredCurvature=0.0)
  initial_path_y = float(road_model.position.y[0])
  for i in range(30):
    road_model.position.y = [initial_path_y - feature.automatic_output] * len(road_model.position.y)
    feature.update(car_state(), True, road_model, controls, now=1.0 + i * 0.2)
  assert abs(feature.last_output - 4 * 0.0254) < 1e-9


def test_center_deadband_ignores_sub_inch_lane_geometry_noise():
  feature = LanePositionController(params({
    "RivianPilotLanePositionPreference": 1,
    "RivianPilotCenterCorrectionInches": 5,
  }))
  establish_straight(feature, model(path_y=0.5 * 0.0254))
  assert feature.filtered_center_error is not None
  assert feature.automatic_output == 0.0
  assert feature.last_output == 0.0


def test_center_filter_rejects_single_frame_direction_reversal():
  feature = LanePositionController(params({
    "RivianPilotLanePositionPreference": 1,
    "RivianPilotCenterCorrectionInches": 5,
  }))
  establish_straight(feature, model(path_y=5 * 0.0254))
  assert feature.filtered_center_error > 0.0
  assert feature.last_output > 0.0

  # Express a five-inch opposite error after removing the path shift caused by
  # the currently published transform. A single noisy frame must not reverse
  # the persistent lane-position request.
  opposite = model(path_y=(-5 * 0.0254) - feature.automatic_output)
  feature.update(car_state(), True, opposite, SimpleNamespace(desiredCurvature=0.0), now=4.0)
  assert feature.filtered_center_error > 0.0
  assert feature.last_output >= 0.0


def test_adaptive_center_limit_ramps_down_at_highway_speed():
  feature = LanePositionController(params({
    "RivianPilotAdaptiveCenterCorrection": True,
    "RivianPilotCenterCorrectionInches": 10,
    "RivianPilotHighwayCenterCorrectionInches": 3,
  }))
  assert feature._adaptive_center_limit_m(10 * 0.44704) == 0.0
  assert abs(feature._adaptive_center_limit_m(30 * 0.44704) - 10 * 0.0254) < 1e-9
  assert abs(feature._adaptive_center_limit_m(55 * 0.44704) - 3 * 0.0254) < 1e-9


def test_center_direction_reversal_requires_stability_and_zero_crossing():
  feature = LanePositionController(params({"RivianPilotLanePositionPreference": 1}))
  correction, state = feature._stable_center_correction(0.1, 1.0)
  assert correction > 0.0 and state == "stable"
  feature.automatic_output = 0.1
  correction, state = feature._stable_center_correction(-0.1, 1.2)
  assert correction == 0.0 and state == "reversal_wait"
  correction, state = feature._stable_center_correction(-0.1, 3.0)
  assert correction == 0.0 and state == "reversal_zero_crossing"
  feature.automatic_output = 0.0
  correction, state = feature._stable_center_correction(-0.1, 3.2)
  assert correction < 0.0 and state == "reversal_accepted"


def test_nudge_countdown_is_published_without_chime_or_control_dependency():
  persistent = params()
  memory = MagicMock()
  feature = LanePositionController(persistent, memory)
  feature.nudge_direction = 1
  feature.nudge_started_at = 1.0
  feature.nudge_until = 11.0
  feature._publish_nudge_timer(1.2)
  memory.put.assert_any_call("RivianPilotNudgeTimerDirection", "left")
  memory.put.assert_any_call("RivianPilotNudgeTimerRemaining", 10)
  memory.put.assert_any_call("RivianPilotNudgeTimerHeartbeat", 1.2)


def test_active_offset_display_reports_applied_direction_and_limiter():
  persistent = params()
  memory = MagicMock()
  feature = LanePositionController(persistent, memory)

  feature._publish_active_offset_status(-6 * 0.0254, -3.5 * 0.0254, "automatic_curve_referenced",
                                        "boundary_clearance", "curve_priority", 2.0)

  memory.put.assert_any_call("RivianPilotActiveOffsetTitle", "Curve Offset • Right 3.5 in")
  memory.put.assert_any_call("RivianPilotActiveOffsetDetail", "Boundary limited • requested 6.0 in")
  memory.put.assert_any_call("RivianPilotActiveOffsetHeartbeat", 2.0)


def test_active_offset_display_reports_blocked_geometry_without_changing_control():
  persistent = params()
  memory = MagicMock()
  feature = LanePositionController(persistent, memory)

  feature._publish_active_offset_status(4 * 0.0254, 0.0, "lane_position_fork_hold",
                                        "fork_hold", "stable", 3.0)

  memory.put.assert_any_call("RivianPilotActiveOffsetTitle", "Lane Position • Left blocked")
  memory.put.assert_any_call("RivianPilotActiveOffsetDetail", "Fork detected • requested 4.0 in")


def test_active_offset_display_failure_is_suppressed():
  persistent = params()
  memory = MagicMock()
  memory.put.side_effect = RuntimeError("display unavailable")
  feature = LanePositionController(persistent, memory)

  feature._publish_active_offset_status(3 * 0.0254, 2 * 0.0254, "manual_authoritative",
                                        "inactive", "inactive", 4.0)

  assert not feature.faulted
  assert feature.last_output == 0.0


def test_active_offset_display_preserves_manual_nudge_countdown():
  persistent = params()
  memory = MagicMock()
  feature = LanePositionController(persistent, memory)
  feature.nudge_until = 70.0

  feature._publish_active_offset_status(3 * 0.0254, 3 * 0.0254, "manual_authoritative",
                                        "inactive", "inactive", 5.0)

  memory.put.assert_any_call("RivianPilotActiveOffsetTitle", "Manual Nudge • Left 3.0 in")
  memory.put.assert_any_call("RivianPilotActiveOffsetDetail", "1:05 remaining")


def test_center_correction_plus_bias_can_reach_fifteen_inches():
  feature = LanePositionController(params({
    "RivianPilotLanePositionPreference": 2,
    "RivianPilotCenterCorrectionInches": 10,
    "RivianPilotLanePositionBiasInches": 5,
  }))
  cs = car_state()
  road_model = model(path_y=10 * 0.0254)
  controls = SimpleNamespace(desiredCurvature=0.0)
  initial_path_y = float(road_model.position.y[0])
  for i in range(40):
    road_model.position.y = [initial_path_y - feature.automatic_output] * len(road_model.position.y)
    feature.update(cs, True, road_model, controls, now=1.0 + i * 0.2)
  assert abs(feature.last_output - 15 * 0.0254) < 1e-9


def test_combined_lane_position_request_is_capped_at_fifteen_inches():
  feature = LanePositionController(params({
    "RivianPilotLanePositionPreference": 2,
    "RivianPilotCenterCorrectionInches": 10,
    "RivianPilotLanePositionBiasInches": 10,
  }))
  cs = car_state()
  road_model = model(path_y=10 * 0.0254)
  controls = SimpleNamespace(desiredCurvature=0.0)
  initial_path_y = float(road_model.position.y[0])
  for i in range(40):
    road_model.position.y = [initial_path_y - feature.automatic_output] * len(road_model.position.y)
    feature.update(cs, True, road_model, controls, now=1.0 + i * 0.2)
  assert abs(feature.last_output - 15 * 0.0254) < 1e-9


def test_observe_only_does_not_compensate_for_unpublished_output():
  p = params({
    "RivianPilotLanePositionPreference": 1,
    "RivianPilotCenterCorrectionInches": 10,
  })
  original_get_bool = p.get_bool.side_effect
  p.get_bool.side_effect = lambda key: False if key == "RivianPilotLanePositionGoLive" else original_get_bool(key)
  feature = LanePositionController(p)
  road_model = model(path_y=0.1)
  controls = SimpleNamespace(desiredCurvature=0.0)
  for i in range(12):
    feature.update(car_state(), True, road_model, controls, now=1.0 + i * 0.2)
  assert feature.last_output == 0.0
  # Observe mode may still ramp its diagnostic target, but must calculate each
  # frame from the unchanged model path rather than feeding back unpublished output.
  assert feature.automatic_output > 0.0
  output_after_first_run = feature.automatic_output
  for i in range(12):
    feature.update(car_state(), True, road_model, controls, now=10.0 + i * 0.2)
  assert feature.last_output == 0.0
  assert feature.automatic_output >= output_after_first_run


def test_enabling_go_live_resets_observed_ramp_before_publishing():
  p = params({
    "RivianPilotLanePositionPreference": 2,
    "RivianPilotCenterCorrectionInches": 10,
    "RivianPilotLanePositionBiasInches": 5,
  })
  go_live = [False]
  original_get_bool = p.get_bool.side_effect
  p.get_bool.side_effect = lambda key: go_live[0] if key == "RivianPilotLanePositionGoLive" else original_get_bool(key)
  feature = LanePositionController(p)
  establish_straight(feature, model(path_y=10 * 0.0254))
  assert feature.automatic_output > 0.0
  assert feature.last_output == 0.0

  go_live[0] = True
  feature.update(car_state(), True, model(path_y=10 * 0.0254), SimpleNamespace(desiredCurvature=0.0), now=4.5)
  assert feature.last_output == 0.0
  assert feature.curve_reference_clearance is None


def test_malformed_numeric_param_uses_safe_default():
  p = params({"RivianPilotCenterCorrectionInches": "invalid"})
  feature = LanePositionController(p)
  assert feature.center_correction_inches == 5
  assert not feature.faulted


def test_left_and_right_bias_apply_on_straight_road():
  left = LanePositionController(params({
    "RivianPilotLanePositionPreference": 2,
    "RivianPilotLanePositionBiasInches": 3,
  }))
  right = LanePositionController(params({
    "RivianPilotLanePositionPreference": 3,
    "RivianPilotLanePositionBiasInches": 3,
  }))
  establish_straight(left)
  establish_straight(right)
  assert abs(left.last_output - 3 * 0.0254) < 1e-9
  assert abs(right.last_output - (-3 * 0.0254)) < 1e-9


def test_lane_preference_falls_back_when_geometry_is_untrusted():
  feature = LanePositionController(params({
    "RivianPilotLanePositionPreference": 2,
    "RivianPilotLanePositionBiasInches": 5,
  }))
  establish_straight(feature)
  assert feature.last_output > 0.0
  controls = SimpleNamespace(desiredCurvature=0.0)
  for i in range(8):
    feature.update(car_state(), True, model(probability=0.2), controls, now=4.0 + i * 0.2)
  assert feature.last_output == 0.0


def test_nudge_hold_time_supports_thirty_minutes_without_changing_nudge_logic():
  feature = LanePositionController(params({"RivianPilotNudgeHoldSeconds": 1800}))
  assert feature.nudge_hold_seconds == 1800
  capped = LanePositionController(params({"RivianPilotNudgeHoldSeconds": 9999}))
  assert capped.nudge_hold_seconds == 1800


def test_live_bias_direction_change_revalidates_opposite_clearance():
  feature = LanePositionController(params())
  cs = car_state()
  controls = establish_curve(feature, cs, curvature=0.002)
  assert feature.last_output > 0.0
  assert feature.curve_reference_clearance is not None

  # Releasing the curve and selecting a right preference changes movement
  # direction. The old positive-side reference cannot be reused for movement
  # in the opposite direction.
  feature.curve_enabled = False
  feature.lane_position_preference = 3
  feature.lane_position_bias_inches = 5
  feature.update(cs, True, model(), controls, now=2.6)
  assert len(feature.curve_reference_samples) == 1
  assert feature.curve_reference_clearance is None
  for now in (2.8, 3.0, 3.2):
    feature.update(cs, True, model(), controls, now=now)
  assert feature.last_output < 0.0


def test_sharp_curve_uses_faster_bounded_ramp():
  p = params({"RivianPilotCurveOffsetInches": 5})
  normal = LanePositionController(p)
  sharp = LanePositionController(p)
  cs = car_state()
  normal.update(cs, True, model(), SimpleNamespace(desiredCurvature=0.0011), now=1.0)
  sharp.update(cs, True, model(), SimpleNamespace(desiredCurvature=0.003), now=1.0)
  # First samples collect geometry. Once the same reference is ready, sharper
  # curvature is allowed to approach the same bounded target more quickly.
  for now in (1.2, 1.4, 1.6):
    normal.update(cs, True, model(), SimpleNamespace(desiredCurvature=0.0011), now=now)
    sharp.update(cs, True, model(), SimpleNamespace(desiredCurvature=0.003), now=now)
  assert abs(sharp.last_output) > abs(normal.last_output)
  assert abs(sharp.last_output) <= 5 * 0.0254


def test_manual_nudge_cancels_and_suppresses_automatic_curve():
  p = params()
  feature = LanePositionController(p)
  cs = car_state()
  controls = establish_curve(feature, cs)
  assert feature.last_output > 0.0

  cs.steeringPressed = True
  cs.steeringTorque = 2.0
  feature.update(cs, True, model(), controls, now=2.6)
  assert feature.last_output == 0.0
  assert not feature.curve_active

  cs.steeringPressed = False
  cs.steeringTorque = 0.0
  feature.update(cs, True, model(), controls, now=2.8)
  assert feature.last_output == 3 * 0.0254
  assert not feature.curve_active


def test_nudge_uses_configured_threshold_logs_peak_and_activates_after_release():
  feature = LanePositionController(params({"RivianPilotNudgeTorqueThresholdTenths": 7}))
  feature._log = MagicMock()
  cs = car_state()
  controls = SimpleNamespace(desiredCurvature=0.0)
  cs.steeringPressed = True
  cs.steeringTorque = 0.69
  feature.update(cs, True, model(), controls, now=1.0)
  assert feature.pending_nudge_direction == 0
  cs.steeringTorque = 0.8
  feature.update(cs, True, model(), controls, now=1.2)
  cs.steeringTorque = 1.1
  feature.update(cs, True, model(), controls, now=1.4)
  assert feature.nudge_direction == 0
  cs.steeringPressed = False
  cs.steeringTorque = 0.0
  feature.update(cs, True, model(), controls, now=1.6)
  assert feature.nudge_direction == 1
  assert feature.last_output == 3 * 0.0254
  feature._log.assert_any_call("nudge_started", direction=1, hold_seconds=10,
                               magnitude_inches=3,
                               trigger_peak_torque=1.1, configured_torque_threshold=0.7,
                               activation_after_release=True, cancellation_grace_seconds=0.5)


def test_nudge_grace_preserves_latch_but_never_fights_driver_input():
  feature = LanePositionController(params())
  cs = car_state()
  controls = SimpleNamespace(desiredCurvature=0.0)
  cs.steeringPressed = True
  cs.steeringTorque = 1.0
  feature.update(cs, True, model(), controls, now=1.0)
  cs.steeringPressed = False
  cs.steeringTorque = 0.0
  feature.update(cs, True, model(), controls, now=1.2)
  assert feature.nudge_direction == 1
  cs.steeringPressed = True
  cs.steeringTorque = 0.2
  feature.update(cs, True, model(), controls, now=1.4)
  assert feature.last_output == 0.0
  assert feature.nudge_direction == 1
  cs.steeringPressed = False
  cs.steeringTorque = 0.0
  feature.update(cs, True, model(), controls, now=1.6)
  assert feature.last_output == 3 * 0.0254


def test_wide_lane_never_exceeds_configured_curve_offset():
  p = params()
  feature = LanePositionController(p)
  establish_curve(feature, road_model=model(lane_half_width=2.0, path_y=-0.5))
  assert feature.last_output == 3 * 0.0254


def test_curve_clearance_filter_compensates_for_its_own_offset():
  p = params()
  feature = LanePositionController(p)
  cs = car_state()
  controls = SimpleNamespace(desiredCurvature=0.002)
  original_model = model(lane_half_width=1.8, path_y=0.0)
  for now in (1.0, 1.2, 1.4, 1.9, 2.4):
    feature.update(cs, True, original_model, controls, now=now)
  reference = feature.curve_reference_clearance
  assert reference is not None
  assert feature.last_output == 3 * 0.0254

  # A future planned-path change is diagnostic and cannot falsely reduce the
  # near-field physical-clearance reference or the approved output.
  feature.update(cs, True, model(lane_half_width=1.8, path_y=abs(feature.last_output)), controls, now=2.6)
  assert feature.curve_reference_clearance >= reference
  assert feature.last_output == 3 * 0.0254


def test_bad_initial_near_field_clearance_recovers_during_same_curve():
  p = params()
  feature = LanePositionController(p)
  cs = car_state()
  controls = SimpleNamespace(desiredCurvature=0.002)
  # Near-field lane geometry reports no physical room on the corrected
  # positive-output movement side.
  blocked = model(lane_half_width=1.5, near_lane_center=0.5)
  for now in (1.0, 1.2, 1.4, 1.6, 1.8):
    feature.update(cs, True, blocked, controls, now=now)
  assert feature.last_output == 0.0

  # Unlike the old frozen reference, a recovered corridor becomes usable
  # without waiting for the curve to end.
  recovered = model(lane_half_width=1.8, path_y=0.0)
  for now in (2.0, 2.2, 2.4, 2.6, 2.8, 3.0):
    feature.update(cs, True, recovered, controls, now=now)
  assert feature.last_output > 0.0


def test_stable_predicted_curve_activates_before_current_curvature():
  p = params()
  feature = LanePositionController(p)
  cs = car_state()
  straight_controls = SimpleNamespace(desiredCurvature=0.0)
  approaching_curve = model(predicted_lat_accel=1.5)
  for now in (1.0, 1.2, 1.4, 1.6, 1.8, 2.0):
    feature.update(cs, True, approaching_curve, straight_controls, now=now)
  assert feature.curve_active
  assert feature.last_output > 0.0


def test_unstable_predicted_direction_never_activates():
  p = params()
  feature = LanePositionController(p)
  cs = car_state()
  controls = SimpleNamespace(desiredCurvature=0.0)
  for i, now in enumerate((1.0, 1.2, 1.4, 1.6, 1.8, 2.0)):
    prediction = 1.5 if i % 2 == 0 else -1.5
    feature.update(cs, True, model(predicted_lat_accel=prediction), controls, now=now)
  assert not feature.curve_active
  assert feature.last_output == 0.0


def test_established_curve_direction_cannot_reverse_mid_episode():
  feature = LanePositionController(params())
  cs = car_state()
  establish_curve(feature, cs, curvature=0.002)
  episode = feature.curve_episode_id
  assert feature.last_output > 0.0
  right_controls = SimpleNamespace(desiredCurvature=-0.002)
  for now in (2.6, 2.8, 3.0):
    feature.update(cs, True, model(predicted_lat_accel=-2.0), right_controls, now=now)
  assert feature.curve_episode_id == episode
  assert feature.curve_direction == 1
  assert feature.last_output > 0.0


def test_fork_holds_custom_offset_then_recovers_on_stable_branch():
  p = params()
  feature = LanePositionController(p)
  cs = car_state()
  controls = establish_curve(feature, cs)
  assert feature.last_output > 0.0

  fork = model(lane_half_width=1.5, far_lane_half_width=2.0)
  for now in (2.6, 2.8, 3.0):
    feature.update(cs, True, fork, controls, now=now)
  assert feature.fork_hold
  assert feature.last_output == 0.0

  stable_branch = model(lane_half_width=1.8)
  for now in (3.2, 3.4, 3.6, 3.8, 4.0, 4.2, 4.4, 4.6, 4.8):
    feature.update(cs, True, stable_branch, controls, now=now)
  assert not feature.fork_hold
  assert feature.last_output > 0.0


def test_relaxed_geometry_never_overrides_fork_hold():
  p = params()
  original_get_bool = p.get_bool.side_effect
  p.get_bool.side_effect = lambda key: True if key == "RivianPilotLanePositionRelaxed" else original_get_bool(key)
  feature = LanePositionController(p)
  cs = car_state()
  controls = establish_curve(feature, cs)
  fork = model(lane_half_width=1.5, far_lane_half_width=2.0)
  for now in (2.6, 2.8, 3.0, 3.2):
    feature.update(cs, True, fork, controls, now=now)
  assert feature.fork_hold
  assert feature.last_output == 0.0


def test_persistent_geometry_loss_ramps_automatic_offset_out():
  p = params()
  feature = LanePositionController(p)
  cs = car_state()
  controls = establish_curve(feature, cs)
  assert feature.last_output > 0.0

  weak_model = model(probability=0.2)
  feature.update(cs, True, weak_model, controls, now=2.6)
  feature.update(cs, True, weak_model, controls, now=2.8)
  assert feature.last_output > 0.0
  feature.update(cs, True, weak_model, controls, now=3.0)
  assert 0.0 < feature.last_output < 3 * 0.0254
  feature.update(cs, True, weak_model, controls, now=3.2)
  assert feature.last_output == 0.0


def test_curve_hysteresis_releases_below_eighty_percent_threshold():
  p = params()
  feature = LanePositionController(p)
  cs = car_state()
  establish_curve(feature, cs)
  assert feature.curve_active

  # Thirty percent strength remains active below the 35% entry threshold but
  # above the 28% release threshold.
  feature.update(cs, True, model(), SimpleNamespace(desiredCurvature=0.001), now=2.6)
  assert feature.curve_active
  feature.update(cs, True, model(), SimpleNamespace(desiredCurvature=0.0005), now=3.0)
  assert not feature.curve_active
  assert feature.last_output == 0.0


def test_observe_only_never_publishes_nonzero():
  p = params()
  p.get_bool.side_effect = lambda key: False if key == "RivianPilotLanePositionGoLive" else bool({
    "RivianPilotLanePositionObserve": True, "RivianPilotFeatureLogging": False,
    "RivianPilotCurveOffset": True, "RivianPilotLanePositionRelaxed": False, "RivianPilotNudgeOffset": True,
  }[key])
  feature = LanePositionController(p)
  establish_curve(feature)
  assert feature.last_output == 0.0


def test_go_live_includes_observation_without_requiring_observe():
  p = params()
  p.get_bool.side_effect = lambda key: bool({
    "RivianPilotLanePositionObserve": False, "RivianPilotLanePositionGoLive": True,
    "RivianPilotFeatureLogging": False, "RivianPilotCurveOffset": True, "RivianPilotLanePositionRelaxed": False,
    "RivianPilotNudgeOffset": True,
  }[key])
  feature = LanePositionController(p)
  establish_curve(feature)
  assert feature.last_output > 0.0


def test_go_live_logs_torque_and_controller_diagnostics(mocker):
  p = params()
  p.get_bool.side_effect = lambda key: bool({
    "RivianPilotLanePositionObserve": True, "RivianPilotLanePositionGoLive": True,
    "RivianPilotFeatureLogging": True, "RivianPilotCurveOffset": True, "RivianPilotLanePositionRelaxed": False,
    "RivianPilotNudgeOffset": True,
  }[key])
  feature = LanePositionController(p)
  cs = car_state()
  cs.steeringTorqueEps = 0.2
  cs.steeringAngleDeg = 1.5
  cs.steeringRateDeg = 0.4
  cs.yawRate = 0.01
  cs.aEgo = -0.1
  lateral_log = SimpleNamespace(saturated=True, output=0.21)
  lateral_state = SimpleNamespace(which=lambda: "torqueState", torqueState=lateral_log)
  controls = SimpleNamespace(desiredCurvature=0.002, curvature=0.0018, lateralControlState=lateral_state)
  car_control = SimpleNamespace(actuators=SimpleNamespace(torque=0.22))
  car_output = SimpleNamespace(actuatorsOutput=SimpleNamespace(torque=0.20))
  event = mocker.patch("openpilot.sunnypilot.rivian_lane_position.lane_position_controller.cloudlog.event")

  feature.update(cs, True, model(), controls, car_control, car_output, now=1.0)

  sample = next(call for call in event.call_args_list if call.kwargs.get("action") == "sample")
  assert sample.kwargs["eps_torque"] == 0.2
  assert sample.kwargs["requested_torque"] == 0.22
  assert sample.kwargs["applied_torque"] == 0.2
  assert sample.kwargs["controller_type"] == "torqueState"
  assert sample.kwargs["controller_saturated"]


def test_logging_omits_none_and_reports_previous_failures(mocker):
  p = params()
  original_get_bool = p.get_bool.side_effect
  p.get_bool.side_effect = lambda key: True if key == "RivianPilotFeatureLogging" else original_get_bool(key)
  event = mocker.patch("openpilot.sunnypilot.rivian_lane_position.lane_position_controller.cloudlog.event")
  error = mocker.patch("openpilot.sunnypilot.rivian_lane_position.lane_position_controller.cloudlog.error")
  feature = LanePositionController(p)
  event.reset_mock()
  event.side_effect = [ValueError("bad diagnostic field"), None]

  feature._log("sample", optional=None, valid=1.0)
  feature._log("sample", optional=None, valid=2.0)

  assert feature.feature_logging
  assert feature.log_failure_count == 1
  assert not feature.faulted
  assert error.call_count == 1
  assert "optional" not in event.call_args.kwargs
  assert event.call_args.kwargs["valid"] == 2.0
  assert event.call_args.kwargs["log_failure_count"] == 1


def test_logging_failure_never_changes_curve_output(mocker):
  p = params()
  original_get_bool = p.get_bool.side_effect
  p.get_bool.side_effect = lambda key: True if key == "RivianPilotFeatureLogging" else original_get_bool(key)
  event = mocker.patch("openpilot.sunnypilot.rivian_lane_position.lane_position_controller.cloudlog.event",
                       side_effect=RuntimeError("logger unavailable"))
  mocker.patch("openpilot.sunnypilot.rivian_lane_position.lane_position_controller.cloudlog.error")
  feature = LanePositionController(p)

  establish_curve(feature)

  assert event.call_count > 1
  assert feature.log_failure_count == event.call_count
  assert feature.feature_logging
  assert not feature.faulted
  assert feature.last_output > 0.0


def test_diagnostic_failure_does_not_disable_lane_position(mocker):
  p = params()
  p.get_bool.side_effect = lambda key: bool({
    "RivianPilotLanePositionObserve": True, "RivianPilotLanePositionGoLive": True,
    "RivianPilotFeatureLogging": True, "RivianPilotCurveOffset": True, "RivianPilotLanePositionRelaxed": False,
    "RivianPilotNudgeOffset": True,
  }[key])
  feature = LanePositionController(p)
  mocker.patch.object(feature, "_diagnostics", side_effect=RuntimeError("diagnostic failure"))

  feature.update(car_state(), True, model(), SimpleNamespace(desiredCurvature=0.002), now=1.0)

  assert feature.diagnostic_faulted
  assert not feature.faulted
  assert feature.last_output == 0.0


def test_blinker_smoothly_hands_offset_to_native_lane_change():
  p = params()
  feature = LanePositionController(p)
  cs = car_state()
  establish_curve(feature, cs)
  assert feature.last_output != 0.0
  cs.leftBlinker = True
  feature.update(cs, True, model(), SimpleNamespace(desiredCurvature=0.002), now=2.6)
  assert 0.0 < feature.last_output < 3 * 0.0254
  previous = feature.last_output
  feature.update(cs, True, model(), SimpleNamespace(desiredCurvature=0.002), now=2.8)
  assert 0.0 <= feature.last_output < previous


def test_native_lane_change_suppresses_all_custom_offsets_and_does_not_restore_nudge():
  feature = LanePositionController(params({"RivianPilotNudgeOffsetInches": 5}))
  cs = car_state()
  feature.nudge_direction = 1
  feature.nudge_magnitude_inches = 5
  feature.nudge_until = 20.0
  feature.update(cs, True, model(), SimpleNamespace(desiredCurvature=0.0), now=1.0)
  assert feature.last_output == 5 * 0.0254
  lane_change_model = model()
  lane_change_model.meta = SimpleNamespace(laneChangeState=2)
  feature.update(cs, True, lane_change_model, SimpleNamespace(desiredCurvature=0.0), now=1.2)
  assert feature.lane_change_suppressed
  assert feature.nudge_direction == 0
  assert 0.0 < feature.last_output < 5 * 0.0254


def _physical_nudge(feature, cs, direction, now):
  cs.steeringPressed = True
  cs.steeringTorque = direction * 1.0
  feature.update(cs, True, model(), SimpleNamespace(desiredCurvature=0.0), now=now)
  cs.steeringPressed = False
  cs.steeringTorque = 0.0
  feature.update(cs, True, model(), SimpleNamespace(desiredCurvature=0.0), now=now + 0.2)


def test_opposite_nudge_neutralizes_before_reversing_and_same_direction_stacks():
  feature = LanePositionController(params({"RivianPilotNudgeOffsetInches": 5}))
  cs = car_state()
  _physical_nudge(feature, cs, 1, 1.0)
  assert feature.nudge_direction == 1
  assert feature.nudge_magnitude_inches == 5
  _physical_nudge(feature, cs, 1, 2.0)
  assert feature.nudge_direction == 1
  assert feature.nudge_magnitude_inches == 6
  _physical_nudge(feature, cs, -1, 3.0)
  assert feature.nudge_direction == 0
  assert feature.last_output == 0.0
  _physical_nudge(feature, cs, -1, 4.0)
  assert feature.nudge_direction == -1
  assert feature.nudge_magnitude_inches == 5


def test_first_nudge_remains_authoritative_but_increment_requires_geometry():
  feature = LanePositionController(params({"RivianPilotNudgeOffsetInches": 5}))
  cs = car_state()
  _physical_nudge(feature, cs, 1, 1.0)
  feature.update(cs, True, model(probability=0.0), SimpleNamespace(desiredCurvature=0.0), now=1.4)
  assert feature.last_output == 5 * 0.0254
  _physical_nudge(feature, cs, 1, 2.0)
  feature.update(cs, True, model(probability=0.0), SimpleNamespace(desiredCurvature=0.0), now=2.4)
  assert feature.nudge_magnitude_inches == 6
  assert feature.last_output == 5 * 0.0254


def test_runtime_failure_falls_back_to_zero_and_stops_heartbeat():
  p = params()
  feature = LanePositionController(p)
  establish_curve(feature)
  assert feature.last_output != 0.0

  heartbeat_writes = sum(call.args[0] == "RivianPilotDynamicCameraOffsetUpdated" for call in p.put.call_args_list)
  feature.suppress_after_error(RuntimeError("injected failure"))

  assert feature.faulted
  assert feature.last_output == 0.0
  zero_write = next(call for call in reversed(p.put.call_args_list)
                    if call.args[0] == "RivianPilotDynamicCameraOffset")
  assert zero_write.args[1] == 0.0
  assert sum(call.args[0] == "RivianPilotDynamicCameraOffsetUpdated" for call in p.put.call_args_list) == heartbeat_writes + 1


def test_params_write_failure_is_contained_by_outer_suppression():
  p = params()
  feature = LanePositionController(p)
  p.put.side_effect = RuntimeError("injected params failure")

  for now in (1.0, 1.2, 1.4):
    try:
      feature.update(car_state(), True, model(), SimpleNamespace(desiredCurvature=0.002), now=now)
    except RuntimeError as e:
      feature.suppress_after_error(e)
      break

  assert feature.faulted
  assert feature.last_output == 0.0
