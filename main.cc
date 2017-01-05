#include <locale>
#include <glibmm/keyfile.h>
#include <gtkmm.h>
#include <stopwatch.h>

int main(int argc, char** argv)
{
	std::locale::global(std::locale(""));
	Glib::KeyFile conf;
	conf.load_from_file("./stopwatch.conf",
			Glib::KeyFileFlags::KEY_FILE_NONE);
	auto app = Gtk::Application::create(argc, argv, "test.stopwatch");
	Stopwatch window(conf);
	return app->run(window.window());
}
