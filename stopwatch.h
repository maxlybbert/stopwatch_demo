#ifndef INCLUDED_STOPWATCH_MAIN_WINDOW_H
#define INCLUDED_STOPWATCH_MAIN_WINDOW_H
#include <chrono>
#include <utility>
#include <gtkmm.h>

class Stopwatch {
	Gtk::Window window_;
	std::pair<Gtk::Button, sigc::connection> start_;
	std::pair<Gtk::Button, sigc::connection> reset_;
	std::pair<Gtk::Button, sigc::connection> timer_;
	sigc::connection clock_timeout_;
	Gtk::Label display_;
	std::chrono::steady_clock::duration running_total_;
	std::chrono::steady_clock::time_point begin_;
	std::chrono::steady_clock::time_point most_recent_;
	const Glib::KeyFile& config_;

	void start_clock();
	void stop_clock();
	void current_ticks();
	void reset_clock();
	bool update_clock();
	bool update_clock(const std::chrono::steady_clock::duration& dur);
	void disconnect_all_signals();
	public:
	Stopwatch(const Glib::KeyFile& config);
	~Stopwatch();
	Gtk::Window& window();
};

inline void Stopwatch::disconnect_all_signals()
{
	sigc::connection signals[] = { start_.second, reset_.second,
		timer_.second, clock_timeout_ };
	for (auto& sig : signals) {
		sig.disconnect();
	}
}

inline Stopwatch::~Stopwatch()
{
	disconnect_all_signals();
}

inline Gtk::Window& Stopwatch::window()
{
	return window_;
}

#endif // INCLUDED_STOPWATCH_MAIN_WINDOW_H

