import math
import platform
import time

from opendbc.car import structs

from openpilot.common.params import Params
from openpilot.common.swaglog import cloudlog


def memory_params() -> Params:
  return Params("/dev/shm/params") if platform.system() != "Darwin" else Params()


INCH_TO_M = 0.0254
# Rivian lists the R1T at 82.0 inches with the mirrors folded and 88.4 inches
# with them open. Painted-lane containment uses the folded-width envelope:
# mirrors can cross a painted line without the truck body or tires leaving it.
DEFAULT_R1T_WIDTH_INCHES = 82
DEFAULT_BOUNDARY_BUFFER_INCHES = 5
DEFAULT_POOR_ROAD_OFFSET_INCHES = 2
MAX_AUTOMATIC_OFFSET_M = 15.0 * INCH_TO_M
MAX_MANUAL_OFFSET_M = 10.0 * INCH_TO_M
SAMPLE_DISTANCE_M = 20.0
MIN_LANE_PROBABILITY = 0.65
MIN_LANE_WIDTH_M = 2.7
MAX_LANE_WIDTH_M = 4.8
MAX_CURVE_LAT_ACCEL = 3.0
NUDGE_TORQUE_THRESHOLD = 1.0
PARAM_REFRESH_SECONDS = 1.0
UPDATE_PERIOD_SECONDS = 0.1
SAMPLE_LOG_PERIOD_SECONDS = 0.5
LOG_ERROR_REPORT_PERIOD_SECONDS = 60.0
CURVE_RELEASE_RATIO = 0.8
CURVE_REFERENCE_SAMPLES = 3
CURVE_CLEARANCE_FILTER_SAMPLES = 5
CURVE_GEOMETRY_MISS_LIMIT = 3
CURVE_RAMP_IN_MPS = 3.0 * INCH_TO_M
STRONG_CURVE_RAMP_IN_MPS = 4.0 * INCH_TO_M
SHARP_CURVE_RAMP_IN_MPS = 5.0 * INCH_TO_M
CURVE_RAMP_OUT_MPS = 8.0 * INCH_TO_M
PREDICTIVE_LOOKAHEAD_START_S = 0.5
PREDICTIVE_LOOKAHEAD_END_S = 2.5
PREDICTIVE_DIRECTION_STABLE_FRAMES = 3
FORK_WIDTH_DELTA_M = 0.65
FORK_RECOVERY_STABLE_FRAMES = 5
MIN_CORRIDOR_SAMPLE_M = 10.0
MAX_CORRIDOR_HORIZON_M = 70.0


class LanePositionController:
  """Compute a bounded temporary model camera offset.

  The controller cannot send steering commands. Go Live only publishes a temporary
  offset consumed by modeld; every invalid or uncertain state publishes zero.
  """

  def __init__(self, params: Params | None = None, params_memory: Params | None = None):
    self.params = params or Params()
    self.params_memory = params_memory if params_memory is not None else (params if params is not None else memory_params())
    self.observe = False
    self.go_live = False
    self.feature_logging = False
    self.curve_enabled = True
    self.relaxed_geometry = False
    self.nudge_enabled = True
    self.curve_offset_inches = 3
    self.curve_threshold_pct = 35
    self.lane_position_preference = 0
    self.center_correction_inches = 5
    self.lane_position_bias_inches = 3
    self.nudge_offset_inches = 3
    self.nudge_hold_seconds = 10
    self.correction_alert_enabled = False
    self.vehicle_width_inches = DEFAULT_R1T_WIDTH_INCHES
    self.boundary_buffer_inches = DEFAULT_BOUNDARY_BUFFER_INCHES
    self.poor_road_offset_inches = DEFAULT_POOR_ROAD_OFFSET_INCHES
    self.last_alert_episode = 0
    self.last_param_read = 0.0
    self.last_update = 0.0
    self.last_heartbeat = 0.0
    self.last_sample_log = 0.0
    self.last_output = None
    self.nudge_direction = 0
    self.nudge_until = 0.0
    self.pending_nudge_direction = 0
    self.curve_active = False
    self.curve_direction = 0
    self.curve_episode_id = 0
    self.curve_episode_started = 0.0
    self.curve_reference_samples = []
    self.curve_reference_clearance = None
    self.curve_geometry_misses = 0
    self.fork_hold = False
    self.fork_recovery_frames = 0
    self.predicted_curve_direction = 0
    self.predicted_curve_stable_frames = 0
    self.automatic_request_direction = 0
    self.automatic_output = 0.0
    self.faulted = False
    self.error_logged = False
    self.log_failure_count = 0
    self.last_log_error_at = None
    self.diagnostic_faulted = False
    self.diagnostic_error_logged = False
    self.get_params()
    self._publish(0.0)
    self._log("initialized", observe=self.observe, go_live=self.go_live)

  @staticmethod
  def _finite(value) -> float:
    value = float(value)
    if not math.isfinite(value):
      raise ValueError("non-finite lane-position input")
    return value

  def _bounded_int_param(self, key: str, default: int, minimum: int, maximum: int) -> int:
    try:
      return max(minimum, min(maximum, int(self.params.get(key, return_default=True))))
    except (KeyError, TypeError, ValueError, OverflowError):
      return default

  def _bool_param(self, key: str, default: bool) -> bool:
    try:
      return bool(self.params.get_bool(key))
    except (KeyError, TypeError, ValueError, OverflowError):
      return default

  def get_params(self) -> None:
    self.observe = self._bool_param("RivianPilotLanePositionObserve", False)
    self.go_live = self._bool_param("RivianPilotLanePositionGoLive", False)
    self.feature_logging = self._bool_param("RivianPilotFeatureLogging", False)
    self.curve_enabled = self._bool_param("RivianPilotCurveOffset", True)
    self.relaxed_geometry = self._bool_param("RivianPilotLanePositionRelaxed", False)
    self.nudge_enabled = self._bool_param("RivianPilotNudgeOffset", True)
    self.curve_offset_inches = self._bounded_int_param("RivianPilotCurveOffsetInches", 3, 1, 10)
    self.curve_threshold_pct = self._bounded_int_param("RivianPilotCurveThreshold", 35, 10, 90)
    self.lane_position_preference = self._bounded_int_param("RivianPilotLanePositionPreference", 0, 0, 3)
    self.center_correction_inches = self._bounded_int_param("RivianPilotCenterCorrectionInches", 5, 1, 10)
    self.lane_position_bias_inches = self._bounded_int_param("RivianPilotLanePositionBiasInches", 3, 1, 10)
    self.nudge_offset_inches = self._bounded_int_param("RivianPilotNudgeOffsetInches", 3, 2, 10)
    self.nudge_hold_seconds = self._bounded_int_param("RivianPilotNudgeHoldSeconds", 10, 5, 1800)
    self.correction_alert_enabled = self._bool_param("RivianPilotLaneCorrectionAlert", False)
    self.vehicle_width_inches = self._bounded_int_param("RivianPilotVehicleWidthInches", DEFAULT_R1T_WIDTH_INCHES, 78, 86)
    self.boundary_buffer_inches = self._bounded_int_param("RivianPilotBoundaryBufferInches", DEFAULT_BOUNDARY_BUFFER_INCHES, 3, 12)
    self.poor_road_offset_inches = self._bounded_int_param("RivianPilotPoorRoadOffsetInches", DEFAULT_POOR_ROAD_OFFSET_INCHES, 1, 10)

  def _publish_correction_alert(self, target_m: float, source: str, now: float) -> None:
    if (not self.correction_alert_enabled or not self.go_live or
        not source.startswith("automatic_curve") or abs(target_m) < 0.5 * INCH_TO_M or
        self.last_alert_episode == self.curve_episode_id):
      return
    try:
      direction = "left" if target_m > 0.0 else "right"
      inches = max(1, min(10, int(round(abs(target_m) / INCH_TO_M))))
      self.params_memory.put("RivianPilotLaneCorrectionAlertDirection", direction)
      self.params_memory.put("RivianPilotLaneCorrectionAlertInches", inches)
      self.params_memory.put("RivianPilotLaneCorrectionAlertAt", float(now))
      self.last_alert_episode = self.curve_episode_id
      self._log("correction_alert", episode_id=self.curve_episode_id, direction=direction,
                approved_inches=inches, source=source)
    except Exception as e:
      # Notification is optional and must never affect the offset controller.
      self._log("correction_alert_failure_suppressed", error_type=type(e).__name__)

  def _log(self, action: str, **kwargs) -> None:
    if not self.feature_logging:
      return
    # Optional diagnostic values are represented by a missing key. Keeping
    # unsupported values out of the structured record prevents a diagnostic
    # serialization problem from affecting future records.
    fields = {key: value for key, value in kwargs.items() if value is not None}
    fields["log_failure_count"] = self.log_failure_count
    try:
      cloudlog.event("rivianpilot lane position", action=action, **fields)
    except Exception as e:
      # Logging is observational and must never alter lane-position state or
      # permanently disable itself after one malformed/dropped record.
      self.log_failure_count += 1
      now = time.monotonic()
      if self.last_log_error_at is None or now - self.last_log_error_at >= LOG_ERROR_REPORT_PERIOD_SECONDS:
        self.last_log_error_at = now
        try:
          cloudlog.error(f"rivianpilot lane position logging failure: {type(e).__name__}")
        except Exception:
          pass

  def _publish(self, offset_m: float) -> None:
    # Go Live always includes observation. Do not require both toggles or run a
    # second copy of the feature when both are enabled.
    try:
      output = self._finite(offset_m) if self.go_live and not self.faulted else 0.0
      if abs(output) > MAX_AUTOMATIC_OFFSET_M:
        raise ValueError("custom offset exceeds R1T limit")
      if self.last_output is None or abs(output - self.last_output) >= 0.001:
        self.params.put("RivianPilotDynamicCameraOffset", float(round(output, 4)), block=False)
        self.last_output = output
      now = time.monotonic()
      if now - self.last_heartbeat >= 0.5:
        self.params.put("RivianPilotDynamicCameraOffsetUpdated", float(now), block=False)
        self.last_heartbeat = now
    except Exception:
      # Stop refreshing the heartbeat. modeld rejects the stale custom offset
      # within one second and resumes unmodified core behavior.
      self.faulted = True
      self.last_output = 0.0
      raise

  def _sample_geometry(self, model, corridor_horizon_m: float = SAMPLE_DISTANCE_M) -> tuple[
    float, float, float, float, float, float, float, float, float, float, float,
  ]:
    probs = list(model.laneLineProbs)
    lines = list(model.laneLines)
    path_x = list(model.position.x)
    path_y = list(model.position.y)
    if len(probs) < 3 or len(lines) < 3 or min(probs[1], probs[2]) < MIN_LANE_PROBABILITY:
      raise ValueError("lane confidence")
    left_x, left_y = list(lines[1].x), list(lines[1].y)
    right_y = list(lines[2].y)
    usable = min(len(left_x), len(left_y), len(right_y))
    if usable == 0 or not path_x or not path_y:
      raise ValueError("lane geometry")
    def sample_at(distance_m: float):
      lane_index = min(range(usable), key=lambda i: abs(float(left_x[i]) - distance_m))
      path_index = min(range(min(len(path_x), len(path_y))),
                       key=lambda i: abs(float(path_x[i]) - float(left_x[lane_index])))
      return float(left_y[lane_index]), float(right_y[lane_index]), float(path_y[path_index])

    left, right, path = sample_at(SAMPLE_DISTANCE_M)
    near_left, near_right, _ = sample_at(0.0)
    if not all(math.isfinite(v) for v in (left, right, path, near_left, near_right)):
      raise ValueError("non-finite geometry")
    # Live model coordinates increase toward driver-right: laneLines[1] is the
    # current lane's left boundary and laneLines[2] is its right boundary.
    if left >= right:
      raise ValueError("lane orientation")
    width = right - left
    if not MIN_LANE_WIDTH_M <= width <= MAX_LANE_WIDTH_M:
      raise ValueError("implausible lane width")
    near_width = near_right - near_left
    if near_left >= near_right or not MIN_LANE_WIDTH_M <= near_width <= MAX_LANE_WIDTH_M:
      raise ValueError("implausible near lane width")
    corridor_widths = []
    for distance_m in (MIN_CORRIDOR_SAMPLE_M, SAMPLE_DISTANCE_M,
                       max(SAMPLE_DISTANCE_M, min(MAX_CORRIDOR_HORIZON_M, corridor_horizon_m))):
      corridor_left, corridor_right, corridor_path = sample_at(distance_m)
      if not all(math.isfinite(v) for v in (corridor_left, corridor_right, corridor_path)):
        raise ValueError("non-finite geometry")
      corridor_width = corridor_right - corridor_left
      if not MIN_LANE_WIDTH_M <= corridor_width <= MAX_LANE_WIDTH_M:
        raise ValueError("fork or merge")
      if not corridor_left < corridor_path < corridor_right:
        raise ValueError("fork or merge")
      corridor_widths.append(corridor_width)
    width_delta = max(corridor_widths) - min(corridor_widths)
    if width_delta > FORK_WIDTH_DELTA_M:
      raise ValueError("fork or merge")
    half_vehicle_width_m = self.vehicle_width_inches * INCH_TO_M / 2.0
    boundary_buffer_m = self.boundary_buffer_inches * INCH_TO_M
    # position.y at the lookahead is the model's future planned path, not the
    # truck's current physical position. Use the near-field lane boundaries
    # around vehicle center (y=0) for movement clearance, while retaining the
    # planned-path clearances for diagnostics and fork/corridor validation.
    planned_left_clearance = path - left - half_vehicle_width_m - boundary_buffer_m
    planned_right_clearance = right - path - half_vehicle_width_m - boundary_buffer_m
    vehicle_left_clearance = -near_left - half_vehicle_width_m - boundary_buffer_m
    vehicle_right_clearance = near_right - half_vehicle_width_m - boundary_buffer_m
    lane_center = (left + right) / 2.0
    return (width, path, lane_center, max(0.0, planned_left_clearance), max(0.0, planned_right_clearance),
            max(0.0, vehicle_left_clearance), max(0.0, vehicle_right_clearance),
            float(probs[1]), float(probs[2]), width_delta, near_width)

  def _sample_geometry_if_authoritative(self, model, corridor_horizon_m: float):
    try:
      return self._sample_geometry(model, corridor_horizon_m), "authoritative"
    except ValueError as e:
      # Lane geometry is an optional guard for automatic movement. Weak or
      # missing lane lines never cancel an authoritative manual nudge.
      return None, str(e)

  @staticmethod
  def _predicted_curve(model) -> tuple[float, float]:
    """Return strongest predicted lateral acceleration and its time in the near horizon."""
    try:
      times = list(model.position.t)
      yaw_rates = list(model.orientationRate.z)
      speeds = list(model.velocity.x)
      usable = min(len(times), len(yaw_rates), len(speeds))
      candidates = []
      for i in range(usable):
        t = float(times[i])
        yaw_rate = float(yaw_rates[i])
        speed = max(0.0, float(speeds[i]))
        if not all(math.isfinite(v) for v in (t, yaw_rate, speed)):
          continue
        if PREDICTIVE_LOOKAHEAD_START_S <= t <= PREDICTIVE_LOOKAHEAD_END_S:
          candidates.append((yaw_rate * speed, t))
      return max(candidates, key=lambda value: abs(value[0])) if candidates else (0.0, 0.0)
    except (AttributeError, TypeError, ValueError, OverflowError):
      return 0.0, 0.0

  @staticmethod
  def _diagnostics(CS, model, controls, car_control=None, car_output=None) -> dict:
    """Best-effort diagnostic fields; unavailable inputs never affect control."""
    diagnostics = {}
    try:
      diagnostics["driver_torque"] = round(float(CS.steeringTorque), 3)
      diagnostics["eps_torque"] = round(float(getattr(CS, "steeringTorqueEps", 0.0)), 3)
      diagnostics["steering_angle_deg"] = round(float(getattr(CS, "steeringAngleDeg", 0.0)), 3)
      diagnostics["steering_rate_deg_s"] = round(float(getattr(CS, "steeringRateDeg", 0.0)), 3)
      diagnostics["yaw_rate_rad_s"] = round(float(getattr(CS, "yawRate", 0.0)), 4)
      diagnostics["accel_ms2"] = round(float(getattr(CS, "aEgo", 0.0)), 3)
    except (TypeError, ValueError, OverflowError):
      pass
    try:
      actual_curvature = float(controls.curvature)
      desired_curvature = float(controls.desiredCurvature)
      if math.isfinite(actual_curvature) and math.isfinite(desired_curvature):
        diagnostics["actual_curvature"] = round(actual_curvature, 7)
        diagnostics["desired_curvature"] = round(desired_curvature, 7)
        diagnostics["actual_lateral_accel_ms2"] = round(actual_curvature * float(CS.vEgo) ** 2, 3)
        diagnostics["desired_lateral_accel_ms2"] = round(desired_curvature * float(CS.vEgo) ** 2, 3)
      lateral_state = getattr(controls, "lateralControlState", None)
      controller_type = lateral_state.which() if lateral_state is not None else "unavailable"
      diagnostics["controller_type"] = controller_type
      lateral_log = getattr(lateral_state, controller_type) if lateral_state is not None else None
      diagnostics["controller_saturated"] = bool(getattr(lateral_log, "saturated", False))
      diagnostics["controller_output"] = round(float(getattr(lateral_log, "output", 0.0)), 4)
    except (AttributeError, TypeError, ValueError, OverflowError):
      pass
    try:
      diagnostics["requested_torque"] = round(float(car_control.actuators.torque), 4)
    except (AttributeError, TypeError, ValueError, OverflowError):
      pass
    try:
      diagnostics["applied_torque"] = round(float(car_output.actuatorsOutput.torque), 4)
    except (AttributeError, TypeError, ValueError, OverflowError):
      pass
    try:
      edge_stds = list(getattr(model, "roadEdgeStds", []))
      diagnostics["left_road_edge_std"] = round(float(edge_stds[0]), 3) if len(edge_stds) >= 2 else None
      diagnostics["right_road_edge_std"] = round(float(edge_stds[1]), 3) if len(edge_stds) >= 2 else None
    except (TypeError, ValueError, OverflowError):
      pass
    return diagnostics

  def _safe_diagnostics(self, CS, model, controls, car_control=None, car_output=None) -> dict:
    if self.diagnostic_faulted:
      return {}
    try:
      return self._diagnostics(CS, model, controls, car_control, car_output)
    except Exception as e:
      self.diagnostic_faulted = True
      if not self.diagnostic_error_logged:
        self.diagnostic_error_logged = True
        try:
          cloudlog.event("rivianpilot feature error", feature="lane_position_diagnostics",
                         errors=["diagnostic_failure_suppressed"], error_type=type(e).__name__)
        except Exception:
          pass
      return {}

  def _reset(self, reason: str) -> None:
    had_offset = self.last_output not in (None, 0.0) or self.nudge_direction != 0 or self.curve_active
    self.nudge_direction = 0
    self.nudge_until = 0.0
    self.pending_nudge_direction = 0
    self.curve_active = False
    self.curve_direction = 0
    self.curve_episode_started = 0.0
    self.curve_reference_samples = []
    self.curve_reference_clearance = None
    self.curve_geometry_misses = 0
    self.fork_hold = False
    self.fork_recovery_frames = 0
    self.predicted_curve_direction = 0
    self.predicted_curve_stable_frames = 0
    self.automatic_request_direction = 0
    self.automatic_output = 0.0
    self._publish(0.0)
    if had_offset:
      self._log("reset", reason=reason)

  def update(self, CS: structs.CarState, lat_active: bool, model, controls, car_control=None,
             car_output=None, now: float | None = None) -> None:
    now = time.monotonic() if now is None else now
    if now - self.last_param_read >= PARAM_REFRESH_SECONDS:
      previous_go_live = self.go_live
      self.get_params()
      self.last_param_read = now
      if self.go_live and not previous_go_live:
        # Observation may have a fully ramped simulated output. A live session
        # must always start from zero and build a fresh geometry reference.
        self._reset("go_live_enabled")
    if self.faulted or now - self.last_update < UPDATE_PERIOD_SECONDS:
      return
    update_dt = min(0.5, max(UPDATE_PERIOD_SECONDS, now - self.last_update)) if self.last_update > 0.0 else UPDATE_PERIOD_SECONDS
    self.last_update = now

    if not (self.observe or self.go_live):
      self._reset("disabled")
      return
    if CS.gearShifter != structs.CarState.GearShifter.drive or not lat_active:
      self._reset("inactive")
      return
    if CS.leftBlinker or CS.rightBlinker:
      self._reset("blinker")
      return

    torque = self._finite(CS.steeringTorque)
    if CS.steeringPressed:
      self.curve_active = False
      self.curve_direction = 0
      self.curve_reference_samples = []
      self.curve_reference_clearance = None
      self.curve_geometry_misses = 0
      self.fork_hold = False
      self.fork_recovery_frames = 0
      self.automatic_output = 0.0
      self.automatic_request_direction = 0
      self.nudge_direction = 0
      self.nudge_until = 0.0
      self.pending_nudge_direction = 1 if torque > NUDGE_TORQUE_THRESHOLD else -1 if torque < -NUDGE_TORQUE_THRESHOLD else 0
      self._publish(0.0)
      return
    if self.nudge_enabled and self.pending_nudge_direction:
      self.nudge_direction = self.pending_nudge_direction
      self.nudge_until = now + self.nudge_hold_seconds
      self.pending_nudge_direction = 0
      self._log("nudge_latched", direction=self.nudge_direction, hold_seconds=self.nudge_hold_seconds)

    desired_curvature = self._finite(controls.desiredCurvature)
    lat_accel = desired_curvature * max(self._finite(CS.vEgo), 0.0) ** 2
    current_curve_strength = min(100.0, abs(lat_accel) / MAX_CURVE_LAT_ACCEL * 100.0)
    predicted_lat_accel, predicted_curve_time = self._predicted_curve(model)
    predicted_curve_strength = min(100.0, abs(predicted_lat_accel) / MAX_CURVE_LAT_ACCEL * 100.0)
    predicted_direction = 1 if predicted_lat_accel > 0.0 else -1 if predicted_lat_accel < 0.0 else 0
    if predicted_direction and predicted_curve_strength >= self.curve_threshold_pct:
      if predicted_direction == self.predicted_curve_direction:
        self.predicted_curve_stable_frames += 1
      else:
        self.predicted_curve_direction = predicted_direction
        self.predicted_curve_stable_frames = 1
    else:
      self.predicted_curve_direction = 0
      self.predicted_curve_stable_frames = 0
    predictive_stable = self.predicted_curve_stable_frames >= PREDICTIVE_DIRECTION_STABLE_FRAMES
    current_direction = 1 if lat_accel > 0.0 else -1 if lat_accel < 0.0 else 0
    current_curve_established = current_curve_strength >= self.curve_threshold_pct * CURVE_RELEASE_RATIO
    # Never let a future opposite bend pull against a curve the truck is
    # currently traversing. Prediction may strengthen the same bend or pre-arm
    # the next bend only after current curvature has released.
    prediction_direction_compatible = predicted_direction == current_direction or not current_curve_established
    use_prediction = predictive_stable and prediction_direction_compatible and predicted_curve_strength > current_curve_strength
    curve_lat_accel = predicted_lat_accel if use_prediction else lat_accel
    curve_strength = predicted_curve_strength if use_prediction else current_curve_strength

    requested = 0.0
    source = "none"
    geometry = None
    geometry_status = "not_needed"
    base_offset = 0.0
    movement_clearance = None
    safety_capped = False
    limiter_reason = "inactive"
    ramp_in_mps = CURVE_RAMP_IN_MPS
    curve_avoidance_request = 0.0
    lane_position_request = 0.0
    measured_lane_center = None
    model_center_error = None
    compensated_center_error = None
    center_correction = 0.0
    reference_state = "not_needed"
    target = 0.0
    base_camera_offset = 0.0
    try:
      base_camera_offset = self._finite(self.params.get("CameraOffset", return_default=True))
    except (TypeError, ValueError, OverflowError):
      base_camera_offset = 0.0
    if self.nudge_direction and now < self.nudge_until:
      # Manual nudge is an explicit driver request. It is never canceled,
      # delayed, or capped by model lane confidence or geometry.
      requested = self.nudge_direction * self.nudge_offset_inches * INCH_TO_M
      requested = max(-MAX_MANUAL_OFFSET_M, min(MAX_MANUAL_OFFSET_M, requested))
      applied = requested
      source = "manual_authoritative"
      self.curve_active = False
      self.curve_direction = 0
      self.curve_reference_samples = []
      self.curve_reference_clearance = None
      self.curve_geometry_misses = 0
      self.fork_hold = False
      self.fork_recovery_frames = 0
      self.automatic_output = 0.0
    else:
      self.nudge_direction = 0
      release_threshold = self.curve_threshold_pct * CURVE_RELEASE_RATIO
      was_curve_active = self.curve_active
      next_curve_active = self.curve_enabled and (
        curve_strength >= (release_threshold if self.curve_active else self.curve_threshold_pct)
      )
      next_curve_direction = 1 if curve_lat_accel > 0.0 else -1 if curve_lat_accel < 0.0 else 0
      new_episode = next_curve_active and (
        not was_curve_active or next_curve_direction != self.curve_direction
      )
      self.curve_active = next_curve_active
      if new_episode:
        self.curve_episode_id += 1
        self.curve_episode_started = now
        self.curve_direction = next_curve_direction
        self.curve_reference_samples = []
        self.curve_reference_clearance = None
        self.curve_geometry_misses = 0
        self.fork_hold = False
        self.fork_recovery_frames = 0
        self.automatic_output = 0.0
        self.automatic_request_direction = 0
        self._log("curve_started", episode_id=self.curve_episode_id,
                  turn_direction="left" if next_curve_direction > 0 else "right",
                  configured_offset_inches=self.curve_offset_inches,
                  configured_threshold_pct=self.curve_threshold_pct,
                  configured_lane_position=self.lane_position_preference,
                  configured_center_correction_inches=self.center_correction_inches,
                  configured_lane_position_inches=self.lane_position_bias_inches,
                  base_camera_offset_m=round(base_camera_offset, 4))
      position_active = self.lane_position_preference != 0
      automatic_active = position_active or (self.curve_active and curve_lat_accel != 0.0)
      if automatic_active:
        if self.curve_active and curve_lat_accel != 0.0:
          # Positive CameraOffset moves driver-left. Move opposite the curve:
          # positive/left curvature requests negative/driver-right, and vice versa.
          base_offset = self.curve_offset_inches * INCH_TO_M
          curve_avoidance_request = -math.copysign(base_offset, curve_lat_accel)

        corridor_horizon_m = max(SAMPLE_DISTANCE_M, min(MAX_CORRIDOR_HORIZON_M,
                                                        max(self._finite(CS.vEgo), 0.0) * PREDICTIVE_LOOKAHEAD_END_S))
        geometry, geometry_status = self._sample_geometry_if_authoritative(model, corridor_horizon_m)
        if geometry is not None:
          (width, path, measured_lane_center, planned_left_clearance, planned_right_clearance,
           vehicle_left_clearance, vehicle_right_clearance, left_prob, right_prob, width_delta, near_width) = geometry
          model_center_error = path - measured_lane_center
          # The model path reacts to our published transform. Add the current
          # output back so persistent positioning converges instead of
          # canceling itself on the next model frame.
          published_feedback = self.last_output or 0.0
          compensated_center_error = model_center_error + published_feedback
          center_limit_m = self.center_correction_inches * INCH_TO_M
          center_correction = max(-center_limit_m, min(center_limit_m, compensated_center_error))
          bias_m = self.lane_position_bias_inches * INCH_TO_M
          if self.lane_position_preference == 1:  # measured center
            lane_position_request = center_correction
          elif self.lane_position_preference == 2:  # driver-left of measured center
            lane_position_request = center_correction + bias_m
          elif self.lane_position_preference == 3:  # driver-right of measured center
            lane_position_request = center_correction - bias_m

        # Curve avoidance is a one-sided minimum safety requirement. A lane
        # preference may move farther toward the outside, but can never pull
        # the target back toward the inside of the active curve.
        requested = lane_position_request
        if curve_avoidance_request > 0.0:
          requested = max(requested, curve_avoidance_request)
        elif curve_avoidance_request < 0.0:
          requested = min(requested, curve_avoidance_request)
        requested = max(-MAX_AUTOMATIC_OFFSET_M, min(MAX_AUTOMATIC_OFFSET_M, requested))
        request_direction = 1 if requested > 0.0 else -1 if requested < 0.0 else 0
        if request_direction != self.automatic_request_direction:
          previous_direction = self.automatic_request_direction
          self.curve_reference_samples = []
          self.curve_reference_clearance = None
          self.curve_geometry_misses = 0
          self.fork_recovery_frames = 0
          self.automatic_request_direction = request_direction
          if previous_direction != 0:
            self._log("movement_direction_changed", episode_id=self.curve_episode_id,
                      previous_direction=previous_direction, requested_direction=request_direction,
                      configured_lane_position=self.lane_position_preference)
        source = "automatic_curve_referenced" if self.curve_active else "lane_position_referenced"
        if geometry is not None:
          movement_clearance = vehicle_left_clearance if requested > 0.0 else vehicle_right_clearance
          self.curve_geometry_misses = 0
          if self.fork_hold:
            self.fork_recovery_frames += 1
            if self.fork_recovery_frames >= FORK_RECOVERY_STABLE_FRAMES:
              self.fork_hold = False
              self.fork_recovery_frames = 0
              self.curve_reference_samples = []
              self.curve_reference_clearance = None
              self._log("fork_recovered", episode_id=self.curve_episode_id)
          if not self.fork_hold:
            # The model path reacts to the published camera transform. Add the
            # current output back before filtering so the feature cannot cancel
            # itself, while allowing a bad initial sample to recover.
            vehicle_width_m = self.vehicle_width_inches * INCH_TO_M
            boundary_buffer_m = self.boundary_buffer_inches * INCH_TO_M
            total_clearance_budget = max(0.0, near_width - vehicle_width_m - 2.0 * boundary_buffer_m)
            published_feedback = self.last_output or 0.0
            compensated_clearance = min(total_clearance_budget,
                                        max(0.0, movement_clearance + abs(published_feedback)))
            self.curve_reference_samples.append(compensated_clearance)
            self.curve_reference_samples = self.curve_reference_samples[-CURVE_CLEARANCE_FILTER_SAMPLES:]
            if len(self.curve_reference_samples) >= CURVE_REFERENCE_SAMPLES:
              ordered = sorted(self.curve_reference_samples)
              self.curve_reference_clearance = ordered[len(ordered) // 2]
              if len(self.curve_reference_samples) == CURVE_REFERENCE_SAMPLES:
                self._log("curve_reference_ready", episode_id=self.curve_episode_id,
                          reference_clearance_m=round(self.curve_reference_clearance, 4),
                          lane_width_m=round(width, 3))
          reference_state = "ready" if self.curve_reference_clearance is not None else "collecting"
        else:
          self.curve_geometry_misses += 1
          reference_state = "missing"
          if geometry_status == "fork or merge":
            if not self.fork_hold:
              self._log("fork_hold", episode_id=self.curve_episode_id)
            self.fork_hold = True
            self.fork_recovery_frames = 0
            self.curve_reference_samples = []
            self.curve_reference_clearance = None
            source = "automatic_curve_fork_hold" if self.curve_active else "lane_position_fork_hold"

        if not self.fork_hold and self.curve_reference_clearance is not None and self.curve_geometry_misses < CURVE_GEOMETRY_MISS_LIMIT:
          target = math.copysign(min(abs(requested), self.curve_reference_clearance), requested)
          safety_capped = abs(target) + 1e-6 < abs(requested)
          limiter_reason = "boundary_clearance" if safety_capped else "approved"
        elif (self.curve_active and not self.fork_hold and self.relaxed_geometry and
              self.curve_geometry_misses >= CURVE_GEOMETRY_MISS_LIMIT):
          # Explicit opt-in for poorly marked roads. The configured request is
          # bounded again below and remains subordinate to driver steering and
          # core control.
          # The driver selects up to ten inches, but weak geometry cannot prove
          # opposite-boundary clearance. Keep the unverified effective cap at
          # three inches; logs and alerts always report the approved value.
          unverified_cap_m = min(self.poor_road_offset_inches, 3) * INCH_TO_M
          target = math.copysign(min(abs(requested), unverified_cap_m), requested)
          reference_state = "relaxed"
          safety_capped = abs(target) + 1e-6 < abs(requested)
          limiter_reason = "poor_road_cap" if safety_capped else "poor_road_approved"
        else:
          # Automatic movement waits for a clean painted-lane reference and
          # ramps out after persistent geometry loss. Core behavior remains.
          target = 0.0
          safety_capped = True
          limiter_reason = "fork_hold" if self.fork_hold else "geometry_wait"

        if self.curve_active and curve_strength >= 75.0:
          ramp_in_mps = SHARP_CURVE_RAMP_IN_MPS
        elif self.curve_active and curve_strength >= 55.0:
          ramp_in_mps = STRONG_CURVE_RAMP_IN_MPS
        rate = ramp_in_mps if abs(target) > abs(self.automatic_output) else CURVE_RAMP_OUT_MPS
        max_step = rate * update_dt
        delta = max(-max_step, min(max_step, target - self.automatic_output))
        self.automatic_output += delta
        if abs(self.automatic_output - target) > 1e-6 and limiter_reason == "approved":
          limiter_reason = "ramp_in"
        if abs(self.automatic_output) < 1e-6:
          self.automatic_output = 0.0
        applied = self.automatic_output
      else:
        target = 0.0
        limiter_reason = "curve_release"
        reference_state = "releasing" if abs(self.automatic_output) > 0.0 else "inactive"
        max_step = CURVE_RAMP_OUT_MPS * update_dt
        delta = max(-max_step, min(max_step, target - self.automatic_output))
        self.automatic_output += delta
        if abs(self.automatic_output) < 1e-6:
          self.automatic_output = 0.0
          self.curve_direction = 0
          self.curve_episode_started = 0.0
          self.curve_reference_samples = []
          self.curve_reference_clearance = None
          self.curve_geometry_misses = 0
          self.fork_hold = False
          self.fork_recovery_frames = 0
          self.automatic_request_direction = 0
        applied = self.automatic_output

    self._publish(applied)
    self._publish_correction_alert(target, source, now)
    diagnostics = self._safe_diagnostics(CS, model, controls, car_control, car_output) if self.feature_logging else {}
    if now - self.last_sample_log >= SAMPLE_LOG_PERIOD_SECONDS:
      lane_fields = {}
      if geometry is not None:
        (width, path, measured_lane_center, planned_left_clearance, planned_right_clearance,
         vehicle_left_clearance, vehicle_right_clearance, left_prob, right_prob, width_delta, near_width) = geometry
        lane_fields = {
          "lane_width_m": round(width, 3),
          "near_lane_width_m": round(near_width, 3),
          "path_y_m": round(path, 3),
          "measured_lane_center_m": round(measured_lane_center, 3),
          "driver_left_clearance_m": round(vehicle_left_clearance, 3),
          "driver_right_clearance_m": round(vehicle_right_clearance, 3),
          "planned_path_left_clearance_m": round(planned_left_clearance, 3),
          "planned_path_right_clearance_m": round(planned_right_clearance, 3),
          "left_lane_probability": round(left_prob, 3),
          "right_lane_probability": round(right_prob, 3),
          "corridor_width_delta_m": round(width_delta, 3),
        }
      self._log("sample", source=source, speed_ms=round(float(CS.vEgo), 3),
                turn_direction="left" if curve_lat_accel > 0.0 else "right" if curve_lat_accel < 0.0 else "straight",
                movement_direction="left" if applied > 0.0 else "right" if applied < 0.0 else "none",
                curve_strength_pct=round(curve_strength, 1),
                current_curve_strength_pct=round(current_curve_strength, 1),
                predicted_curve_strength_pct=round(predicted_curve_strength, 1),
                predicted_curve_time_s=round(predicted_curve_time, 2) if predicted_curve_time else None,
                predictive_curve_used=use_prediction,
                episode_id=self.curve_episode_id if source.startswith("automatic_curve") else None,
                episode_age_s=round(now - self.curve_episode_started, 2) if self.curve_episode_started else None,
                configured_offset_inches=self.curve_offset_inches,
                configured_threshold_pct=self.curve_threshold_pct,
                configured_lane_position=self.lane_position_preference,
                configured_center_correction_inches=self.center_correction_inches,
                configured_lane_position_inches=self.lane_position_bias_inches,
                configured_vehicle_width_inches=self.vehicle_width_inches,
                configured_boundary_buffer_inches=self.boundary_buffer_inches,
                configured_poor_road_offset_inches=self.poor_road_offset_inches,
                relaxed_geometry=self.relaxed_geometry,
                base_offset_m=round(base_offset, 4),
                curve_avoidance_request_m=round(curve_avoidance_request, 4),
                lane_position_request_m=round(lane_position_request, 4),
                model_center_error_m=round(model_center_error, 4) if model_center_error is not None else None,
                compensated_center_error_m=round(compensated_center_error, 4)
                if compensated_center_error is not None else None,
                center_correction_m=round(center_correction, 4),
                requested_offset_m=round(requested, 4),
                movement_clearance_m=round(movement_clearance, 4) if movement_clearance is not None else None,
                reference_clearance_m=round(self.curve_reference_clearance, 4)
                if self.curve_reference_clearance is not None else None,
                reference_state=reference_state,
                fork_hold=self.fork_hold, fork_recovery_frames=self.fork_recovery_frames,
                geometry_miss_count=self.curve_geometry_misses,
                limiter_reason=limiter_reason,
                ramp_in_inches_per_second=round(ramp_in_mps / INCH_TO_M, 1),
                safety_capped_offset_m=round(applied, 4), safety_capped=safety_capped,
                published_offset_m=round(self.last_output or 0.0, 4),
                base_camera_offset_m=round(base_camera_offset, 4),
                effective_target_offset_m=round(base_camera_offset + applied, 4),
                geometry_status=geometry_status,
                r1t_lane_envelope_m=round(self.vehicle_width_inches * INCH_TO_M, 4),
                boundary_margin_m=round(self.boundary_buffer_inches * INCH_TO_M, 4),
                approved_target_offset_m=round(target, 4), go_live=self.go_live, observe=self.observe,
                **lane_fields, **diagnostics)
      self.last_sample_log = now

  def suppress_after_error(self, exception: Exception) -> None:
    self.faulted = True
    try:
      self.params.put("RivianPilotDynamicCameraOffset", 0.0, block=False)
      self.params.put("RivianPilotDynamicCameraOffsetUpdated", 0.0, block=False)
      self.last_output = 0.0
    except Exception:
      # A stale heartbeat independently forces modeld back to the core offset.
      pass
    if self.error_logged:
      return
    self.error_logged = True
    try:
      cloudlog.event("rivianpilot feature error", feature="lane_position",
                     errors=["runtime_failure_suppressed"], error_type=type(exception).__name__)
    except Exception:
      pass
