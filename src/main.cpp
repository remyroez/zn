#include <chrono>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

#include <argparse/argparse.hpp>

namespace {

std::string make_filename(const std::chrono::system_clock::time_point&tp = std::chrono::system_clock::now()) {
	const auto local_time = std::chrono::current_zone()->to_local(tp);
	const auto local_time_sec = std::chrono::time_point_cast<std::chrono::seconds>(local_time);
	const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(local_time - local_time_sec);
	return std::format("{:%Y%m%d%H%M%S}{:03d}.md", local_time_sec, ms.count());
}

struct output_result {
	std::optional<std::string> message;
	std::optional<std::string> error;

	operator bool() const {
		return message || error;
	}
};

output_result output_file(std::string_view filename, std::string_view title) {
	output_result result;
	if (std::ofstream output_file(filename.data()); output_file.is_open()) {
		output_file.exceptions(std::ofstream::badbit | std::ofstream::failbit);
		try {
			output_file << "# " << title << std::endl;
			output_file.close();

			result.message = std::format("output {} - {}", filename, (title.empty() ? "<no title>" : title));

		} catch (std::ofstream::failure &err) {
			result.error = std::format("{}", err.what());
		}

	} else {
		result.error = std::format("can't output {}", filename);
	}
	return result;
}

} // namespace

int main(int argc, char **argv) {
	std::string title;
	{
		argparse::ArgumentParser program("zn");

		program.add_argument("title")
			.help("zettel title")
			.default_value("");

		try {
			program.parse_args(argc, argv);
			title = program.get<std::string>("title");

		} catch (const std::exception &err) {
			std::cerr << "error: " << err.what() << std::endl;
			std::cerr << program;
			std::exit(1);
		}
	}

	if (auto result = ::output_file(::make_filename(), title); result) {
		if (result.message) {
			std::cout << "success: " << result.message.value() << std::endl;
		}
		if (result.error) {
			std::cerr << "error: " << result.error.value() << std::endl;
		}
	}

	return 0;
}
