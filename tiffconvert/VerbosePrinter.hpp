#pragma once

#ifndef verbose_printer_h
#define verbose_printer_h
	#include "rang.hpp"
	#include <iostream>
	#include <string>
	#include <vector>
	#include <functional>
	#include <type_traits>
	#include <algorithm>

	#include <Windows.h>

	namespace TiffConvert::Cli {

		template <typename _TOfValue>
		struct is_number : std::conjunction<
			std::is_arithmetic<_TOfValue>,
			std::negation<std::is_same<_TOfValue, bool>>,
			std::negation<std::is_same<_TOfValue, char>>
		> {};

		class VerbosePrinter {
			private:
				std::ostream&	m_Stream;
				std::wostream&	m_WStream;
				bool			m_Colors;
				std::string		m_IndentString;
				std::string		m_Indent = "";

				template <typename _TType>
				inline std::ostream& if_color(const _TType& v) const {
					if (!m_Colors)
						return m_Stream;
					return m_Stream << v;
				}

				template <typename _TType>
				inline std::wostream& wif_color(const _TType& v) const {
					if (!m_Colors) 
						return m_WStream;
					m_Stream << v;
					return m_WStream;
				}

				inline std::ostream& reset() const {
					if (!m_Colors)
						return m_Stream;
					return m_Stream << rang::style::reset;
				}

				static void replace_all(std::string& str, const std::string& from, const std::string& to) {
					size_t start_pos = 0;
					while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
						str.replace(start_pos, from.length(), to);
						start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
					}
				}

				static void replace_all(std::wstring& str, const std::wstring& from, const std::wstring& to) {
					size_t start_pos = 0;
					while ((start_pos = str.find(from, start_pos)) != std::wstring::npos) {
						str.replace(start_pos, from.length(), to);
						start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
					}
				}

				inline std::string escape(const std::string& in) const {
					auto result = in;
					replace_all(result, "\n", "\\n");
					replace_all(result, "\r", "\\r");
					replace_all(result, "\t", "\\t");
					replace_all(result, "\"", "\\\"");
					return result;
				}

				inline std::wstring escape(const std::wstring& in) const {
					auto result = in;
					replace_all(result, L"\n", L"\\n");
					replace_all(result, L"\r", L"\\r");
					replace_all(result, L"\t", L"\\t");
					replace_all(result, L"\"", L"\\\"");
					return result;
				}
			public:
				VerbosePrinter(std::ostream& stream, std::wostream& wstream, bool colors = true, const std::string indentString = "\t") 
					: m_Stream(stream), m_WStream(wstream), m_Colors(colors), m_IndentString(indentString) {}

				inline void Category(const std::string& name) {
					if_color(rang::fg::cyan) << m_Indent << "[";
					if_color(rang::fgB::cyan) << name;
					if_color(rang::fg::cyan) << "] ";
					reset();
				}

				inline void BeginSection(char s = '{') {
					m_Stream << s << "\n";
					m_Indent += m_IndentString;
				}

				inline void BeginSection(const std::string& name, char s = '{') {
					Category(name);
					BeginSection(s);
				}

				inline void EndSection(char s = '}') {
					m_Indent = m_Indent.substr(m_IndentString.size());
					m_Stream << m_Indent << s << "\n";
				}

				inline void Section(std::function<void(void)> func, char s1 = '{', char s2 = '}') {
					BeginSection(s1);
					func();
					EndSection(s2);
				}

				inline void Section(const std::string& name, std::function<void(void)> func, char s1 = '{', char s2 = '}') {
					Category(name);
					Section(func, s1, s2);
				}

				inline void Name(const std::string& name) {
					if_color(rang::fg::cyan) << name << ": ";
					reset();
				}

				inline void Indent() {
					if (!m_Indent.empty())
						m_Stream << m_Indent;
				}

				template <typename _TContained>
				inline void Array(const std::string& type, const std::vector<_TContained>& arr) {
					if_color(rang::fg::magenta) << "Array containing ";
					if_color(rang::fgB::magenta) << arr.size();
					if_color(rang::fg::magenta) << " " << type << (arr.size() != 1 ? "s" : "") << " ";
					reset();
				}

				inline void Rectangle(const RECT& rect) {
					if_color(rang::fgB::green) << "RECT { ";
					Number(rect.left, "px");
					if_color(rang::fgB::green) << ", ";
					Number(rect.top, "px");
					if_color(rang::fgB::green) << ", ";
					Number(rect.right - rect.left, "px");
					if_color(rang::fgB::green) << ", ";
					Number(rect.bottom - rect.top, "px");
					if_color(rang::fgB::green) << " } \n";
					reset();
				}

				inline void Rectangle(const std::string& name, const RECT& rect) {
					Indent();
					Name(name);
					Rectangle(rect);
					reset();
				}

				inline void RgbQuad(const RGBQUAD& rgbQuad) {
					if_color(rang::fgB::green) << "RGB { ";
					Number(static_cast<std::uint32_t>(rgbQuad.rgbRed));
					if_color(rang::fgB::green) << ", ";
					Number(static_cast<std::uint32_t>(rgbQuad.rgbGreen));
					if_color(rang::fgB::green) << ", ";
					Number(static_cast<std::uint32_t>(rgbQuad.rgbBlue));
					if_color(rang::fgB::green) << " } \n";
					reset();
				}

				inline void RgbQuad(const std::string& name, const RGBQUAD& rgbQuad) {
					Indent();
					Name(name);
					RgbQuad(rgbQuad);
					reset();
				}

				template <typename _TNumber, std::enable_if_t<is_number<_TNumber>::value, int> = 0>
				inline void Number(const _TNumber& value, const std::string& unit = "") {
					if_color(rang::fgB::yellow) << value;
					if (!unit.empty())
						if_color(rang::fg::yellow) << unit;
				}

				template <typename _TNumber, std::enable_if_t<is_number<_TNumber>::value, int> = 0>
				inline void Number(const std::string& name, const _TNumber& value, const std::string& unit = "") {
					Indent();
					Name(name);
					Number(value, unit);
					m_Stream << "\n";
					reset();
				}

				template <typename _TNumber, std::enable_if_t<std::conjunction<is_number<_TNumber>, std::negation<std::is_floating_point<_TNumber>>>::value, int> = 0>
				inline void NumberHex(const _TNumber& value, const std::string& unit = "") {
					if_color(rang::fgB::yellow) << "0x" << std::hex << value << std::dec;
					if (!unit.empty())
						if_color(rang::fg::yellow) << unit;
				}

				template <typename _TNumber, std::enable_if_t<std::conjunction<is_number<_TNumber>, std::negation<std::is_floating_point<_TNumber>>>::value, int> = 0>
				inline void NumberHex(const std::string& name, const _TNumber& value, const std::string& unit = "") {
					Indent();
					Name(name);
					NumberHex(value, unit);
					m_Stream << "\n";
					reset();
				}

				inline void Boolean(bool value, const std::string& truthy = "true", const std::string& falsy = "false") {
					if_color(rang::fgB::blue) << (value ? truthy : falsy);
				}

				inline void Boolean(const std::string& name, bool value, const std::string& truthy = "true", const std::string& falsy = "false") {
					Indent();
					Name(name);
					Boolean(value, truthy, falsy);
					m_Stream << "\n";
					reset();
				}

				inline void Point(const POINT& point) {
					if_color(rang::fgB::green) << "POINT { ";
					Number(point.x, "px");
					if_color(rang::fgB::green) << ", ";
					Number(point.y, "px");
					if_color(rang::fgB::green) << " } \n";
					reset();
				}

				inline void Point(const std::string& name, const POINT& point) {
					Indent();
					Name(name);
					Point(point);
					reset();
				}

				inline void Points(const std::vector<POINT>& points) {
					Array("point", points);

					if (points.empty())
						return;

					Section([&]() {
						for (const auto& p : points) {
							m_Stream << m_Indent << " - ";
							Point(p);
						}
					}, '[', ']');

					reset();
				}

				inline void Points(const std::string& name, const std::vector<POINT>& points) {
					Indent();
					Name(name);
					Points(points);

					reset();
				}

				inline void Text(const std::string& value) {
					if_color(rang::fgB::magenta) << "\"" << escape(value) << "\"";
				}

				inline void Text(const std::string& name, const std::string& value) {
					Indent();
					Name(name);
					Text(value);
					m_Stream << "\n";
					reset();
				}

				inline void Text(const std::wstring& value) {
					wif_color(rang::fgB::magenta) << "\"" << escape(value) << "\"";
				}

				inline void Text(const std::string& name, const std::wstring& value) {
					Indent();
					Name(name);
					Text(value);
					m_Stream << "\n";
					reset();
				}
		};

	}

#endif 