#pragma once

#ifndef cli_codec_validator_h
#define cli_codec_validator_h

#include "CLI11.hpp"

namespace TiffConvert {
	namespace Cli {
		/// <summary>
		/// CodecValidator describes a <see cref="CLI::Validator"/> that can check if the specified codec is valid.
		/// </summary>
		struct CodecValidator : public CLI::Validator {
			public:
				static std::set<std::string> Valid;	// a static set of valid codec names.
				static std::string ValidString;		// a string representing the static set of valid codec names.
				static const CodecValidator Validator;	// a static instance of the validator, since only one is required.

			private:
				/// <summary>
				/// The private constructor, this is a singleton.
				/// </summary>
				CodecValidator();
		};
	}
}

#endif 