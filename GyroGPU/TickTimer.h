#pragma once
#include <chrono>

/// <summary>
/// This class keeps track of how much time has elapsed since the last game tick, and when to execute another. 
/// 
/// Typical workflow is this:
///		Call `init` just outside of your main loop. That starts the clock.
///		Every frame, increment the clock with `update`.
///		Then, use `isTickReady` to ask if enough time has elapsed for another game tick. 
///		While it has (yes, a real while loop in case you need two ticks), 
///		Execute the game tick, then call `decrement` to indicate that you actually executed the game tick.
///	
/// </summary>
class TickTimer
{
	std::chrono::high_resolution_clock::time_point previousTime;
	double elapsedTime;
	double secondsPerTick;

public:
	TickTimer(double seconds_per_tick);
	TickTimer(int ticks_per_second);
	TickTimer();

	/// <summary>
	/// Queries the current system time and updates the accumulator `elapsedTime` with the difference between now and `previousTime`
	/// Call once per frame.
	/// 
	/// </summary>
	void update();

	/// <summary>
	/// Indicates when enough time has passed for another game tick to happen. 
	/// Should be followed by `decrement` if true. 
	/// 
	/// </summary>
	/// <returns> true when another game tick is required. </returns>
	bool isTickReady() const;

	/// <summary>
	/// Subtracts the expected time between game ticks from the current elapsed time. 
	/// Call this whenever you execute a game tick. 
	/// 
	/// </summary>
	void decrement();
};

