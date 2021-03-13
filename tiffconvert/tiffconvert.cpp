#include "Util.hpp"
#include "TiffImage.hpp"
#include "Renderer.hpp"
#include "DynaCli.hpp"
#include "CodecValidator.hpp"
#include "Arguments.hpp"
#include "PreRenderWangHandler.hpp"
#include "CompositeWangHandler.hpp"
#include "VerboseWangHandler.hpp"
#include "VerbosePrinter.hpp"

#include <TiffFile.hpp>
#include <WangAnnotationReader.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>

namespace fs = std::filesystem;

using CliContainer      = TiffConvert::Cli::DynaCli<bool, std::string, uint32_t>;    // The DynaCli container for this application.
using TiffImage         = std::shared_ptr<TiffConvert::TiffImage>;                   // A shared pointer variant of TiffImage.
using TiffFile          = std::shared_ptr<TiffWang::Tiff::TiffFile>;                 // A shared pointer variant of TiffFile.
using AnotReader        = TiffWang::Tiff::WangAnnotationReader;                      // The annotation reader.
using PreRenderer       = TiffConvert::Handlers::PreRenderWangHandler;               // The pre-render handler.
using Composition       = TiffConvert::Handlers::CompositeWangHandler;
using VerboseHandler    = TiffConvert::Handlers::VerboseWangHandler;
using HandlerCollection = std::vector<std::shared_ptr<TiffWang::Tiff::IWangAnnotationCallback>>;

// A static mapping from codec name to extension.
static std::unordered_map<std::string, std::string> codec_extension_map = {
    { "png",      "png" },
    { "jpeg",     "jpg" },
    { "jpeg2000", "jp2" },
    { "bitmap",   "bmp" }
};

// A static mapping from codec name to codec.
static std::unordered_map<std::string, tiff_export_format> codec_map = {
    { "png",      tiff_export_format::TIFF_EXPORT_PNG },
    { "jpeg",     tiff_export_format::TIFF_EXPORT_JPEG },
    { "jpeg2000", tiff_export_format::TIFF_EXPORT_JPEG2000 },
    { "bitmap",   tiff_export_format::TIFF_EXPORT_BITMAP }
};

/// <summary>
/// Determine if the parent directory of a path exists.
/// </summary>
/// <param name="path">The path to check.</param>
/// <returns>True when it exists, false otherwise.</returns>
bool parent_directory_exists(const std::string& path) {
    auto parent = fs::path(path).parent_path();
    return fs::exists(parent) && fs::is_directory(parent);
}

/// <summary>
/// Generate a path based on basepath, image index and extension.
/// </summary>
/// <param name="path">Base path.</param>
/// <param name="index">Image index.</param>
/// <param name="extension">Extension.</param>
/// <returns>The newly constructed path.</returns>
std::string path_from_base_index(const std::string& path, size_t index, const std::string& extension) {
    auto parent   = fs::path(path).parent_path();
    auto basename = fs::path(path).stem();

    return fs::absolute(parent.append(basename.string() + "_" + std::to_string(index) + "." + extension)).string();
}

/// <summary>
/// Process the task at hand.
/// </summary>
/// <param name="cli">The main cli options object.</param>
/// <param name="cli_image">The images subcommand cli options object.</param>
/// <param name="cli_pdf">The pdf subcommand cli options object.</param>
/// <param name="image">The decoded Tiff image.</param>
/// <param name="file">The binary representation of the Tiff file.</param>
/// <returns></returns>
int process(const CliContainer& cli, const CliContainer& cli_image, const CliContainer& cli_pdf, TiffImage image, TiffFile file) {
    auto codec       = cli.get<std::string>(TiffConvert::Cli::NAME_OUTCODEC);
    auto verbose     = cli.isset(TiffConvert::Cli::NAME_VERBOSE);
    uint32_t options = (codec.compare("jpeg") == 0 || codec.compare("jpeg2000") == 0) ? 10 : 0;

    std::shared_ptr<TiffConvert::Cli::VerbosePrinter> printer = nullptr;

    // Construct a verbose printer to use for this session
    if (verbose)
        printer = std::make_shared<TiffConvert::Cli::VerbosePrinter>(std::cout, std::wcout, true, "  ");

    // Pass 1: Prerender eiStream/Wang annotations.
    if (cli.isset(TiffConvert::Cli::NAME_PRERENDER)) {
        for (size_t pageIndex = 0; pageIndex < file->GetPageCount(); ++pageIndex) {
            if (verbose) 
                printer->BeginSection("TIFF IFD #" + std::to_string(pageIndex));

            for (size_t ifdIndex = 0; ifdIndex < file->GetPageIfdCount(pageIndex); ifdIndex++) {
                const auto& ifd = file->GetPageIfd(pageIndex, ifdIndex);
                
                if (verbose) {
                    uint32_t    multiple;
                    std::string type;

                    switch (ifd.TagType) {
                        case TiffTagType::SHORT:
                            multiple = 2;
                            type = "SHORT";
                            break;
                        case TiffTagType::LONG:
                            multiple = 4;
                            type = "LONG";
                            break;
                        case TiffTagType::RATIONAL:
                            multiple = 8;
                            type = "RATIONAL";
                            break;
                        case TiffTagType::ASCII:
                        case TiffTagType::BYTE:
                            multiple = 1;
                            type = (ifd.TagType == TiffTagType::ASCII) ? "ASCII" : "BYTE";
                            break;
                        default:
                            multiple = 0;
                            type = "UNKNOWN";
                    }

                    printer->BeginSection("TIFF IFD ENTRY #" + std::to_string(ifdIndex));
                    printer->NumberHex("ID", static_cast<uint16_t>(ifd.TagId));
                    printer->Text("TYPE", type);
                    printer->NumberHex("OFFSET", ifd.ValueOffset, " bytes");
                    printer->Number("COUNT", ifd.ValueCount);
                    printer->Number("SIZE", ifd.ValueCount * multiple);
                    printer->Boolean("IS WANG", ifd.IsWangTag);
                }

                if (!ifd.IsWangTag) {
                    printer->EndSection();
                    continue;
                }
                
                // eiStream/Wang tag found: read and render it.
                image->Render(static_cast<uint32_t>(pageIndex), [&](HDC hDc) {
                    auto renderer = std::make_shared<PreRenderer>(file->GetDimensions(pageIndex), hDc);
                    AnotReader wangReader(*file, ifd);
                    
                    if (!verbose) {
                        // Not verbose, just set the renderer as the only handler 
                        wangReader.SetHandler(renderer);
                    } else {
                        // Verbose, make a composition handler from the verbose handler (logger) and renderer handler 
                        wangReader.SetHandler(std::make_shared<Composition>(HandlerCollection { 
                            std::make_shared<VerboseHandler>(printer),
                            renderer 
                        }));
                    }

                    // Read the eiStream/wang tags and invoke the handlers on significant events. 
                    wangReader.Read();
                });

                printer->EndSection();
            }

            if (verbose)
                printer->EndSection();
        }
    }

    // Pass 2: Invert colors
    if (cli.isset(TiffConvert::Cli::NAME_INVERT)) {
        #pragma warning ( push )
        #pragma warning ( disable: 4100 ) // unreferenced formal parameter, this is a callback function and we don't need all the parameters from the interface.
        auto filter = [](uint64_t x, uint64_t y, uint64_t top, uint64_t bottom) -> uint64_t {
            return bottom ^ 0xffffff;
        };
        #pragma warning ( pop ) 

        for (size_t pageIndex = 0; pageIndex < file->GetPageCount(); ++pageIndex) {
            const auto& pageDimensions = file->GetDimensions(pageIndex);

            if (verbose) {
                printer->Section("INVERT", [&]() {
                    printer->Number("PAGE", pageIndex);
                });
            }

            // Xor the image content with 0xffffff through a filter, effectively causing the colors to invert.
            image->Render(static_cast<uint32_t>(pageIndex), [&] () {
                /* 
                    Use libtiffconvert's internal API call instead of the Renderer class so that we can 
                    specify a custom filter. We basically draw a black rectangle (rgba(0, 0, 0, 255)) 
                    onto the output, which is only used for the metrics. The filter used simply xor's the 
                    bottom color with 0xffffff.
                */
                RECT r{ 0, 0, static_cast<LONG>(pageDimensions.Width), static_cast<LONG>(pageDimensions.Height) };
                renderer_rect(&r, 255U << 24, 0U, true, false, 0U, 0U, filter); 
            });
        }
    }

    // Pass 3: Scale pages.
    if (cli.anyset({ TiffConvert::Cli::NAME_MAXWIDTH, TiffConvert::Cli::NAME_MAXWIDTH })) {
        auto maxwidth  = cli.get_isset_or<uint32_t>(TiffConvert::Cli::NAME_MAXWIDTH, 0);
        auto maxheight = cli.get_isset_or<uint32_t>(TiffConvert::Cli::NAME_MAXHEIGHT, 0);
        auto smooth    = cli.isset(TiffConvert::Cli::NAME_SCALESMOOTH);

        for (size_t pageIndex = 0; pageIndex < file->GetPageCount(); ++pageIndex) {
            if (verbose) {
                printer->Section("SCALE", [&]() {
                    printer->Number("PAGE", pageIndex);
                });
            }

            if (!image->ScaleToMaximum(static_cast<uint32_t>(pageIndex), maxwidth, maxheight, smooth))
                throw std::runtime_error("page scaling failed for page " + std::to_string(pageIndex));
        }
    }

    // Pass 4: Export
    if (cli.get_chosen_subcommand_name() == TiffConvert::Cli::NAME_SUBCOMMAND_IMAGE) {
        auto basepath = cli_image.get<std::string>(TiffConvert::Cli::NAME_OUTBASE);
        if (!parent_directory_exists(basepath))
            throw std::runtime_error("parent path does not exist: " + basepath);

        for (size_t pageIndex = 0; pageIndex < file->GetPageCount(); ++pageIndex) {
            auto target = path_from_base_index(basepath, pageIndex, codec_extension_map.at(codec));

            if (verbose) {
                printer->Section("EXPORT IMAGE", [&]() {
                    printer->Number("PAGE", pageIndex);
                    printer->Text("FILE", target);
                });
            }

            if (!image->ExportPage(static_cast<uint32_t>(pageIndex), target, codec_map.at(codec), options)) {
                throw std::runtime_error("cannot store image");
                return 1;
            }
        }

        if (verbose)
            printer->Section("EXPORT IMAGE", [&]() { printer->Boolean("DONE", true); });

        return 0;
    }  else if (cli.get_chosen_subcommand_name() == TiffConvert::Cli::NAME_SUBCOMMAND_PDF) {
        auto target = cli_pdf.get<std::string>(TiffConvert::Cli::NAME_OUTPDF);

        if (verbose) 
            printer->Section("EXPORT PDF", [&]() { printer->Text("FILE", target); });

        if (!image->ExportPdf(target, codec_map.at(codec), options)) {
            throw std::runtime_error("cannot store pdf");
            return 1;
        }

        if (verbose) 
            printer->Section("EXPORT PDF", [&]() { printer->Boolean("DONE", true); });

        return 0;
    }

    return 0;
}

/// <summary>
/// Main program entry point.
/// </summary>
/// <param name="argc">Program argument count.</param>
/// <param name="argv">Program arguments.</param>
/// <returns>Status code, nonzero means there is a problem.</returns>
int main(int argc, char* argv[]) {
    // Construct the cli options objects.
    CliContainer cli(TiffConvert::Cli::DESC_APPLICATION, TiffConvert::Cli::NAME_APPLICATION);
    cli.command().require_subcommand();
    cli.command().set_help_all_flag(TiffConvert::Cli::DESC_HELPALL.Flag, TiffConvert::Cli::DESC_HELPALL.Desc);
    cli.command().footer("(c) Bas Groothedde, Imagine Programming. MIT Licensed, do whatever.\r\nInclude LICENSE.md from repo in your distributions.");

    // flags and options for the main command 
    cli.add_flag(TiffConvert::Cli::DESC_VERBOSE);
    cli.add_flag(TiffConvert::Cli::DESC_INVERT);
    cli.add_flag(TiffConvert::Cli::DESC_PRERENDER);
    cli.add_option<std::string>(
        TiffConvert::Cli::DESC_OUTCODEC.Name,
        TiffConvert::Cli::DESC_OUTCODEC.Flag,
        TiffConvert::Cli::DESC_OUTCODEC.Desc + TiffConvert::Cli::CodecValidator::ValidString)->check(TiffConvert::Cli::CodecValidator::Validator)->required(true);
    cli.add_option<uint32_t>(TiffConvert::Cli::DESC_MAXWIDTH);
    cli.add_option<uint32_t>(TiffConvert::Cli::DESC_MAXHEIGHT);
    cli.add_flag(TiffConvert::Cli::DESC_SCALESMOOTH);
    cli.add_option<std::string>(TiffConvert::Cli::DESC_TIFFILE)->required(true)->check(CLI::ExistingFile);

    // images command
    auto& image_command = cli.add_subcommand(TiffConvert::Cli::NAME_SUBCOMMAND_IMAGE, TiffConvert::Cli::DESC_SUBCOMMAND_IMAGE);
    image_command.add_option<std::string>(TiffConvert::Cli::DESC_OUTBASE)->required(true);

    // pdf command
    auto& pdf_command = cli.add_subcommand(TiffConvert::Cli::NAME_SUBCOMMAND_PDF, TiffConvert::Cli::DESC_SUBCOMMAND_PDF);
    pdf_command.add_option<std::string>(TiffConvert::Cli::DESC_OUTPDF)->required(true);

    // parse using CLI11
    try {
        cli.command().parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return cli.command().exit(e);
    }

    TiffImage image = nullptr;
    TiffFile  file  = nullptr;
    const std::string& path = cli.get<std::string>(TiffConvert::Cli::NAME_TIFFILE);

    try {
        // try decoding the image and loading the file in binary form.
        image = std::make_shared<TiffConvert::TiffImage>(path);
        file  = std::make_shared<TiffWang::Tiff::TiffFile>(path);

        // try reading the IFD collection, effectively reading the description of each Tiff page.
        file->ReadIfdCollection();
    } catch (const std::exception& ex) {
        std::cout << "error: " << ex.what() << std::endl;
        return 1;
    }

    // No pages? Abort.
    if (image->GetPageCount() == 0) {
        std::cout << "error: cannot find any images in specified tiff file" << std::endl;
        return 1;
    }

    // Page count from binary processing does not match the page count from the decoded image? Abort.
    if (static_cast<size_t>(image->GetPageCount()) != file->GetPageCount()) {
        std::cout << "error: libtiffconvert reported a different page count than libtiffwang, cannot proceed" << std::endl;
        return 1;
    }

    try {
        // Try processing the task at hand.
        return process(cli, image_command, pdf_command, image, file);
    } catch (const std::exception& ex) {
        std::cout << "error: " << ex.what() << std::endl;
        return 1;
    }
}