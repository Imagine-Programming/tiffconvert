#pragma once

#ifndef cli_arguments_h
#define cli_arguments_h

#include "DynaCli.hpp"

/*
	The Arguments.hpp include contains static const expressions for descriptors of CLI arguments.
*/

namespace TiffConvert {
	namespace Cli {
		static constexpr auto NAME_APPLICATION		= "tiffconvert";
		static constexpr auto DESC_APPLICATION		= "Convert legacy TIFF files with or without annotations to commonly used image formats or multi-page PDF files.";

		static constexpr auto NAME_SUBCOMMAND_IMAGE = "images";
		static constexpr auto DESC_SUBCOMMAND_IMAGE = "Output each page as rendered image.";

		static constexpr auto NAME_SUBCOMMAND_PDF   = "pdf";
		static constexpr auto DESC_SUBCOMMAND_PDF   = "Output each page as a page in a resulting PDF file.";

		static constexpr auto NAME_HELPALL = "helpall";
		static constexpr const OptionDescriptor DESC_HELPALL(NAME_HELPALL, "-H,--help-all", "Display the complete help listing for all commands.");

		static constexpr auto NAME_INVERT = "invert";
		static constexpr const OptionDescriptor DESC_INVERT(NAME_INVERT, "-i,--invert-colors", "Invert the colors of each Tiff page.");

		static constexpr auto NAME_PRERENDER = "prerender";
		static constexpr const OptionDescriptor DESC_PRERENDER(NAME_PRERENDER, "-p,--prerender-wang", "Prerender eiStream/WANG tags onto each TIFF page, when present.");

		static constexpr auto NAME_OUTCODEC = "outcodec";
		static constexpr const OptionDescriptor DESC_OUTCODEC(NAME_OUTCODEC, "-c,--codec", "The codec to use when encoding TIFF pages, one of: ");

		static constexpr auto NAME_MAXWIDTH = "maxwidth";
		static constexpr const OptionDescriptor DESC_MAXWIDTH(NAME_MAXWIDTH, "-x,--max-width", "The maxium width in pixels for a single page in the TIFF file.");

		static constexpr auto NAME_MAXHEIGHT = "maxheight";
		static constexpr const OptionDescriptor DESC_MAXHEIGHT(NAME_MAXHEIGHT, "-y,--max-height", "The maxium height in pixels for a single page in the TIFF file.");

		static constexpr auto NAME_SCALESMOOTH = "scalesmooth";
		static constexpr const OptionDescriptor DESC_SCALESMOOTH(NAME_SCALESMOOTH, "-s,--scale-smooth", "Use interpolation when the pages have to be scaled.");

		static constexpr auto NAME_TIFFILE = "tiffpath";
		static constexpr const OptionDescriptor DESC_TIFFILE(NAME_TIFFILE, "tiff-file", "The TIFF image to convert.");

		static constexpr auto NAME_OUTBASE = "outbase";
		static constexpr const OptionDescriptor DESC_OUTBASE(NAME_OUTBASE, "basename", "The base image path for the output.");

		static constexpr auto NAME_OUTPDF = "outpdf";
		static constexpr const OptionDescriptor DESC_OUTPDF(NAME_OUTPDF, "output", "The filepath of the PDF-file to write.");
	}
}

#endif 