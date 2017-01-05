#include <stopwatch.h>
#include <chrono>
#include <cstdio>
#include <memory>
#include <glibmm/main.h>
#include <gtkmm.h>


namespace {
	template<typename Dur_t> static bool format_time(Dur_t time,
			char* buffer, size_t len)
	{
		size_t status = -1;
		auto secs = std::chrono::duration_cast<std::chrono::milliseconds>(
				time).count() / 1000.;
		if (secs < 60) {
			status = std::snprintf(buffer, len, "%.3f", secs);
			return status > 0 && status <= len;
		}
		int mins = static_cast<int>(secs) / 60;
		double seconds = secs - mins * 60;
		status = std::snprintf(buffer, len, "%d:%06.3f", mins, seconds);
		return status > 0 && status <= len;
	}
}

Stopwatch::Stopwatch(const Glib::KeyFile& config)
	: config_(config)
{
	const auto& config_group = config_.get_start_group();
	char buffer[10];
	format_time(std::chrono::milliseconds::zero(), buffer, sizeof buffer);
	display_.set_text(buffer);

	window_.set_default_size(200, 200);
	std::unique_ptr<Gtk::Grid> layout(new Gtk::Grid);
	layout->set_border_width(10);
	layout->set_margin_top(5);
	layout->set_margin_right(5);
	layout->set_margin_bottom(5);
	layout->set_margin_left(5);
	layout->set_valign(Gtk::Align::ALIGN_CENTER);
	layout->set_halign(Gtk::Align::ALIGN_CENTER);

	display_.set_margin_right(5);
	layout->attach(display_, 1, 1, 2, 1);
	reset_.first.set_margin_top(5);
	reset_.first.set_margin_right(2);
	layout->attach(reset_.first, 1, 2, 1, 1);
	start_.first.set_margin_top(5);
	start_.first.set_margin_left(2);
	layout->attach(start_.first, 2, 2, 1, 1);
	window_.add(*layout);
	Gtk::manage(layout.release());
	window_.set_focus_child(start_.first);
	auto conn = start_.first.signal_clicked().connect(
			sigc::mem_fun(this, &Stopwatch::start_clock));
	start_.second = conn;
	start_.first.set_label(config_.get_locale_string(config_group,
				"Start"));
	running_total_ = std::chrono::steady_clock::duration::zero();
	reset_.first.set_label(config_.get_locale_string(config_group,
				"Reset"));
	window_.show_all_children();
}

void Stopwatch::start_clock()
{
	const auto& config_group = config_.get_start_group();
	begin_ = std::chrono::steady_clock::now();
	disconnect_all_signals();
	auto conn = start_.first.signal_clicked().connect(
			sigc::mem_fun(this, &Stopwatch::stop_clock));
	start_.second = conn;
	bool (Stopwatch::*upd)() = &Stopwatch::update_clock;
	conn = Glib::signal_timeout().connect(
			sigc::mem_fun(this, upd), 1);
	timer_.second = conn;
	start_.first.set_label(config_.get_locale_string(config_group, "Stop"));
	conn = reset_.first.signal_clicked().connect(
			sigc::mem_fun(this, &Stopwatch::current_ticks));
	reset_.second = conn;
	reset_.first.set_label(config_.get_locale_string(config_group,
				"Pause"));
}

void Stopwatch::stop_clock()
{
	auto agora = std::chrono::steady_clock::now();
	const auto& config_group = config_.get_start_group();
	auto elapsed = agora - begin_;
	running_total_ += elapsed;
	update_clock(running_total_);
	disconnect_all_signals();
	auto conn = start_.first.signal_clicked().connect(
			sigc::mem_fun(this, &Stopwatch::start_clock));
	start_.second = conn;
	start_.first.set_label(config_.get_locale_string(config_group,
				"Start"));
	conn = reset_.first.signal_clicked().connect(
			sigc::mem_fun(this, &Stopwatch::reset_clock));
	reset_.second = conn;
	reset_.first.set_label(config_.get_locale_string(config_group,
				"Reset"));
}

void Stopwatch::current_ticks()
{
	timer_.second.disconnect();
	auto agora = std::chrono::steady_clock::now();
	auto elapsed = agora - begin_;
	update_clock(elapsed);
}

void Stopwatch::reset_clock()
{
	disconnect_all_signals();
	char buffer[10];
	format_time(std::chrono::milliseconds::zero(), buffer, sizeof buffer);
	display_.set_text(buffer);
	auto conn = start_.first.signal_clicked().connect(
			sigc::mem_fun(this, &Stopwatch::start_clock));
	start_.second = conn;
	running_total_ = std::chrono::steady_clock::duration::zero();
}

bool Stopwatch::update_clock()
{
	auto agora = std::chrono::steady_clock::now();
	auto elapsed = agora - begin_;
	return update_clock(running_total_ + elapsed);
}

bool Stopwatch::update_clock(const std::chrono::steady_clock::duration& dur)
{
	const auto& config_group = config_.get_start_group();
	// size is set to be "large enough that the value becomes inconvenient
	// long before we overflow the buffer"
	char buff[100];
	if (format_time(dur, buff, sizeof buff)) {
		display_.set_text(buff);
		return true;
	}
	display_.set_text(config_.get_locale_string(config_group, "Error"));
	return false;
}
