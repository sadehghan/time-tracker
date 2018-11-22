#ifndef TIME_TRACKER_H_
#define TIME_TRACKER_H_

#include <ctime>
#include <cstdint>
#include <chrono>

/// Performs all timing operations like timeout checking, and the current time tracking.
class TimeTracker
{
public:
	/// Second to microsecond
	static constexpr unsigned int MICROSECONDS = 1000000;

	/// Default constructor of TimeTracker
	TimeTracker()
	: timeout({0, 0})
	, now({0, 0})
	{
	}

	/**
	 * Constructor of TimeTracker
	 *
	 * @param timeout Default timeout value.
	 */
	explicit TimeTracker(timeval timeout)
	: timeout(timeout)
	, now({0, 0})
	{
	}

	/**
	 * Constructor of TimeTracker
	 *
	 * @param timeout Default timeout value in microseconds.
	 */
	explicit TimeTracker(uint32_t timeout_us)
	: timeout({ timeout_us / MICROSECONDS, timeout_us % MICROSECONDS })
	, now({0, 0})
	{
	}

	/**
	 * Constructor of TimeTracker
	 *
	 * @param timeout Default timeout value in microseconds.
	 * @param now Set current time.
	 */
	TimeTracker(time_t timeout_us, timeval now)
	: timeout({ timeout_us / MICROSECONDS, timeout_us % MICROSECONDS })
	, now(now)
	{
	}

	/**
	 * Constructor of TimeTracker
	 *
	 * @param timeout Default timeout value in microseconds.
	 * @param now Set current time.
	 */
	TimeTracker(time_t timeout_us, std::chrono::system_clock::time_point now)
	: timeout({ timeout_us / MICROSECONDS, timeout_us % MICROSECONDS })
	, now({std::chrono::system_clock::to_time_t(now), 0})
	{
	}

	/// Set the current time of this time tracker.
	void set_now(timeval current_time)
	{
		now = current_time;
	}

	/// Set the current time of this time tracker.
	void set_now(std::chrono::system_clock::time_point current_time)
	{
		now = {std::chrono::system_clock::to_time_t(current_time), 0};
	}

	/// @return The current time of this time tracker.
	timeval get_now() const
	{
		return now;
	}

	/**
	 * Set the timeout of this time tracker
	 *
	 * @param timeout value in microseconds.
	 */
	void set_timeout(time_t timeout_us)
	{
		timeout = { timeout_us / MICROSECONDS, timeout_us % MICROSECONDS };
	}

	/// @return The default timeout of this time tracker.
	timeval get_timeout() const
	{
		return timeout;
	}

	/// @return The default timeout of this time tracker in microseconds.
	uint32_t get_timeout_us() const
	{
		return timeout.tv_sec * MICROSECONDS + timeout.tv_usec;
	}

	/**
	 * Check timeout and update current time. Advances the current time if needed.
	 *
	 * @param current_time Current time to check and set as now if after current now.
	 *
	 * @return True if timed out, false otherwise.
	 */
	bool is_timed_out(timeval current_time, timeval timeout)
	{
		if (compare_timevals(now, current_time) > 0)
			return false;

		timeval deadline = add_timevals(now, timeout);
		now = current_time;		// Advance the current time

		return compare_timevals(now, deadline) > 0;
	}

	// Same as is_timed_out(timeval current_time, timeval timeout) except it checks against default timeout
	bool is_timed_out(timeval current_time)
	{
		return is_timed_out(current_time, timeout);
	}

	/**
	 * Check timeout and update current time if and only if timeout.
	 *
	 * @param current_time Current time to check and set as now if timeout.
	 *
	 * @return True if time out, false otherwise.
	 */
	bool is_timed_out_and_update_if_timed_out(timeval current_time)
	{
		if (compare_timevals(now, current_time) > 0)
			return false;

		timeval deadline = add_timevals(now, timeout);
		bool result = compare_timevals(current_time, deadline) > 0;

		if (result)
			now = current_time;

		return result;
	}

	/**
	 * Check timeout and update current time if and only if timeout.
	 *
	 * @param current_time_input Current time to check and set as now if timeout.
	 *
	 * @return True if time out, false otherwise.
	 */
	bool is_timed_out_and_update_if_timed_out(std::chrono::system_clock::time_point current_time_input)
	{
		timeval current_time{std::chrono::system_clock::to_time_t(current_time_input), 0};
		return is_timed_out_and_update_if_timed_out(current_time);
	}

private:
	/**
	 * Compare two timeval structs.
	 *
	 * @param left The left value to comparing.
	 * @param right The right value to comparing.
	 * @return If two time is equal return 0, if left is less than right return -1,
	 * and if left is greater than right return 1.
	 */
	static int compare_timevals(timeval left, timeval right)
	{
		if (left.tv_sec < right.tv_sec)
			return -1;		// Returns -1, if left < right

		if (left.tv_sec > right.tv_sec)
			return 1;		// Returns 1, if  left > right

		if (left.tv_usec < right.tv_usec)
			return -1;		// Returns -1, if left < right

		if (left.tv_usec > right.tv_usec)
			return 1;		// Returns 1, if left > right

		return 0;		// Returns 0, if left == right
	}

	/**
	 * Add two timeval structs.
	 *
	 * @param left The left value.
	 * @param right The right value.
	 * @param result The result of operation.
	 */
	static timeval add_timevals(timeval left, timeval right)
	{
		timeval result;

		result.tv_sec = left.tv_sec + right.tv_sec;
		result.tv_usec = left.tv_usec + right.tv_usec;

		if (result.tv_usec >= MICROSECONDS)
		{
			++result.tv_sec;
			result.tv_usec -= MICROSECONDS;
		}

		return result;
	}

	/**
	 * Timeout value.
	 *
	 * This value used to check timeout.
	 */
	timeval timeout;

	/**
	 * The current time of this time tracker.
	 *
	 * Whenever we use the is_timed_out() function the current time will be advanced if needed.
	 */
	timeval now;
};

#endif
