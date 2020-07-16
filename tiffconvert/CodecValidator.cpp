#include "CodecValidator.hpp"

using namespace TiffConvert::Cli;

// a static set of valid codec names.
std::set<std::string> CodecValidator::Valid = {
	"png", "jpeg", "jpeg2000", "bitmap"
};

// a string representing the static set of valid codec names.
std::string CodecValidator::ValidString = "png, jpeg, jpeg2000, bitmap";

// a static instance of the validator, since only one is required.
const CodecValidator CodecValidator::Validator = CodecValidator();

/// <summary>
/// The private constructor, this is a singleton.
/// </summary>
CodecValidator::CodecValidator() {
	tname = "CODEC";
	func = [](const std::string& str) -> std::string {
		if (!Valid.count(str))
			return "Invalid codec specified: " + str;
		return std::string();
	};
}
