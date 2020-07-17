<p align="center"><img src="./logo/sizes/tiffconvert.256x256.png" alt="tiffconvert logo" /></p>

# tiffconvert
Two libraries and a command-line application for converting legacy Tiff files with eiStream/Wang annotations to other formats, like PDF.

## OS
* Tested on Windows 10, but any PureBasic supported 64-bit OS should do. 
* Not built for 32-bit systems, this is still a goal.

## Features
tiffconvert (conjunction of libtiffconvert and libtiffwang) is a command-line utility program aiming at users of the legacy Tiff image format, with embedded eiStream/Wang marks (annotations). These 
eiStream/Wang marks (annotations) are stored in a Tiff IFD tag, not many free programs I found support these annotations, especially not image viewers that support the Tiff image format. 

* Converting Tiff images to a single PNG, JPEG, JPEG2000 or BMP file per IFD (page);
* Whilst converting, burning the eiStream/Wang marks (annotations) onto the resulting images using `--prerender-wang`;
* Saving the resulting images as individual files or;
* Saving the resulting images as a single PDF;
* After pre-processing, one can invert the colors using `--invert-colors` (useful for some CAD drawings);
* After pre-processing, one can scale the image to a maximum width and height using `--max-width` and/or `--max-height`;
* During scaling, interpolation can optionally be applied using `--scale-smooth`.

## Building and Installation
* Clone the repository and build the PureBasic project first (developed in PureBasic 5.71 LTS x64), this will produce `libtiffconvert.(dll|lib|exp)`. PureBasic was chosen, because 
it includes a rich image and 2D rendering library that provided me with the means of rendering the annotations onto the produced images. 
* Then open the Visual Studio solution file in VS2019 and build the libtiffwang DLL and tiffconvert in order (not at once). tiffconvert references libtiffwang. 
* Then move the 2 DLL files (`libtiffconvert.dll` and `libtiffwang.dll`) and the executable (`tiffconvert.exe`) into one directory, which is your release build.
* You can also download the latest release build, check releases.

## Disclaimer
Disclaimer; the libtiffconvert DLL does not directly wrap the functionality of the image and/or drawing functions, it merely allows operating on Tiff pages for rendering. 
It does allow loading images from buffer and file, due to the fact that eiStream/Wang marks can include image blobs and file references. The referenced files are yet to be implemented.
It should be completely in line with the PureBasic end-user agreement. If this is not the case, I would like the PureBasic staff to contact me so that I can sort it out.

## Usage Examples
### Convert a Tiff with annotations to PDF
... each burnt image encoded with PNG

```bash
tiffconvert -pcpng input-file.tiff pdf output-file.pdf
```

### Convert a Tiff with annotations to individual images
... using a basepath, whilst rescaling and inverting 

```bash
tiffconvert -sipcjpeg -x3000 -y3000 input-file.tiff images output/basename
```

### Asking for help
... and see all the available options

```bash
tiffconvert --help-all
```

## Remarks
eiStream/Wang marks are not fully implemented yet, there's still a few issues to resolve:
* Image and Text marks referencing a file on the filesystem are currently ignored, as I still want to figure out how to do it securely;
* Text rotation is not yet implemented, however PureBasic does have an API for it. I'm unsure about what the center point of rotation and text baseline is in those situations;
* Hyperlinks are ignored, as the burnt images are not clickable. I would like to add a post-processing mode that creates the annotations as PDF annotations, with hyperlinks supported;
* OCR regions are ignored and are not on the TODO-list.

## Dependencies
tiffconvert as a project might depend on other libraries or code;

* [PureBasic 5.71 LTS x64](https://purebasic.com)
* (included in solution) [CLI11](https://github.com/CLIUtils/CLI11);
* (included in project) [Image Rotation Routines by Luis](https://www.purebasic.fr/english/viewtopic.php?f=12&t=38975);
* (included in project) [PureBasic PDF Module by Thorsten1867](https://www.purebasic.fr/english/viewtopic.php?f=12&t=69267), based on PurePDF by (LuckyLuke, ABBKlaus, normeus);
* (included in solution) DynaCli - a simple wrapper I wrote around CLI11, which also stores the values options can have in their defined types;
* (included in project) [CatchTIFFPage by wilbert](https://www.purebasic.fr/english/viewtopic.php?f=13&t=58880#p441476), providing a nice trick to decode each Tiff page individually;
* (included in project) [A line rendering routine by galah](https://www.purebasic.fr/english/viewtopic.php?f=13&t=43570#p334079);
* (included in project) [Another line rendering routine by Michael Vogel](https://www.purebasic.fr/english/viewtopic.php?f=13&t=43570#p333664).

If I forgot something, please let me know.

## Support
Open an issue on GitHub or @ me, [@BGroothedde](https://twitter.com/BGroothedde)

## Development Setup
If you want to make changes in `libtiffconvert`, you need to buy and install PureBasic 5.71 LTS. It's cheap and has lifetime updates, but you don't have to if you're not going to 
change code in that library. You may also open an issue requesting a change in that library and I will make it if I agree.

For the rest; just clone the solution and open it in Visual Studio 2019.

## Release History
- **1.1.1.0**:
    - bugfix: _stricmp result not compared to 0 in endianess mark check, now using memcmp and comparing to 0.
- **1.1.0.0**:
    - initial public source release.

## Meta
- Author: Bas Groothedde ([@BGroothedde](https://twitter.com/BGroothedde), [Blog](https://www.xoru.net))
- Contributors: 
    - Lisa Marie (Logo)

## Documentation
For now, there are two forms of docs - a wiki would be nice in the future:

- `tiffconvert --help-all`
- XML Comments in code, nothing generated from them yet as the support for C++ XML comments is not that good at the moment.

## Contributing
Everyone is welcome to contribute, but please open an issue first to discuss the change you'd like to make. Anything is fine: bug fixes, additions, changes in logic and changes in behaviour. Only the logo is not going to change, probably ... just open an issue first, because I'd like to monitor and co-decide on changes, as this project is actively used in an enterprise environment.

Please follow the same code style as seen in the current source, changes to the style can obviously discussed. Don't use `new`, `delete`, an allocation function and `free` when not needed. Use `std::vector`, `std::array`, `std::unique_ptr` and `std::shared_ptr` or stack allocated instances instead wherever possible. 

- Fork the project;
- Create a feature branch (`git checkout -b feature/name`);
- Commit your changes (`git commit -am 'Add some name'`), please use descriptive messages in your commits;
- Push to the branch (`git push origin feature/name`);
- Create a new Pull Request.

## Desired additions
- Tests;
- Code review / cleanup, it has been developed in a high pace - so there are many best practises to add;
- Bug fixes;
- New useful features that can benefit the core purpose of tiffconvert.

## Built With
- Visual Studio 2019, Enterprise on Windows 10 Professional.
- PureBasic 5.71 LTS x64 on Windows 10 Professional.

## License
[MIT License](LICENSE.md)
