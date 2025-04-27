# Light Scheduler TDD Exercise

A Test-Driven Development (TDD) exercise for implementing a light scheduler that controls room lights based on timed events. Focuses on using mocks, spies, and TDD discipline to build a hardware-agnostic solution.

## Purpose
- Practice TDD by writing tests first, watching them fail, and writing minimal code to pass.
- Replace hardware dependencies (e.g., `TimeService`, `LightControl`) with test doubles (mocks and spies).
- Validate light scheduling logic without physical hardware.

## Core Components
1. **Test Doubles**
   - `LightControlSpy`: Tracks light state changes (on/off) and IDs for validation.
   - `MockTimeService`: Simulates time queries and alarms to trigger scheduled events.
2. **Light Scheduler**
   - Schedules/unschedules light events (on/off) for specific days/times.
   - Supports weekdays, weekends, daily, and custom schedules.
3. **Test Infrastructure**
   - **Unity**: Testing framework for writing assertions.
   - **CMock**: Generates mock objects (e.g., `MockTimeService`).

## Getting Started
### Dependencies
- Ruby (for CMock code generation)
- C compiler (e.g., `gcc`)
- Unity and CMock frameworks (included in the project).

### Build & Run Tests
```bash
make  # Builds and runs all tests (TestLightScheduler, TestLightControlSpy, etc.)
```

## Key Files
- `src/LightScheduler.c`: Production code for scheduling logic.
- `test/TestLightScheduler.c`: Unit tests for scheduler functionality.
- `include/LightControlSpy.h`: Interface for the light control test spy.
- `mocks/MockTimeService.h`: Generated mock for time service.

## TDD Approach
1. **Write Tests First**  
   Example test for scheduling a light:
   ```c
   void testScheduleOnWeekdayTurnsLightOn(void) {
     LightScheduler_init();
     LightScheduler_schedule(42, WEEKDAY, 18, 30, LIGHT_ON);
     MockTimeService_setDay(MONDAY);
     MockTimeService_setTime(18, 30);
     LightScheduler_wakeup(); // Trigger scheduled event
     TEST_ASSERT_EQUAL(42, LightControlSpy_getLastLightId());
     TEST_ASSERT_EQUAL(LIGHT_ON, LightControlSpy_getLastState());
   }
   ```
2. **Watch Tests Fail**
3. **Write Minimal Code**
   ```c
   // src/LightScheduler.c
   void LightScheduler_schedule(int id, Day day, int hour, int minute, int state) {
    // Stub implementation - just enough to pass first test
    if (day == WEEKDAY && hour == 18 && minute == 30) {
        LightControl_on(id);
    }
   }
  

## Key Test Cases

### Basic Scheduling
- Schedule light ON at 18:30 on Monday.
- Verify `LightControlSpy` detects the correct state change.

### Edge Cases
- Schedule at 23:59 on Sunday.
- Schedule overlapping events for the same light.

### Invalid Inputs
- Light ID > 255.
- Time outside 00:00–23:59.

### Event Removal
- Remove a scheduled event and ensure it no longer triggers.

### Capacity Limits
- Schedule 256 events (max) and ensure no overflow.  
