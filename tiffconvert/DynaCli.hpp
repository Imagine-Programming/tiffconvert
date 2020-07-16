#pragma once 

#ifndef dynacli_h
#define dynacli_h

#include "cli11.hpp"
#include <unordered_map>
#include <type_traits>
#include <functional>
#include <optional>
#include <variant>
#include <memory>
#include <string>
#include <cstdint>

namespace TiffConvert {
    namespace Cli {
        namespace detail {
            /// <summary>
            /// An internal type to keep track of command options, flags and their values.
            /// </summary>
            /// <typeparam name="TTypes">a list of types of values that can be held by DynaCli</typeparam>
            template <typename ...TTypes>
            struct InternalOption {
                CLI::Option*            Option;
                std::variant<TTypes...> Value;
            };

            /*
                contains<...> can be used to determine if a variadic template argument pack contains a specific type.
                It is used in hindsight to ensure 'bool' is in that list, so that DynaCli can handle flags.
            */

            template <typename T, typename... Args>
            struct contains;

            template <typename T>
            struct contains<T> : std::false_type {};

            template <typename T, typename... Args>
            struct contains<T, T, Args...> : std::true_type {};

            template <typename T, typename A, typename... Args>
            struct contains<T, A, Args...> : contains<T, Args...> {};

            template <typename T, typename... Args>
            constexpr bool contains_v = contains<T, Args...>::value;
        }

        /// <summary>
        /// OptionDescriptor is a struct that should describe command line flags and options.
        /// See ArgumentNames.hpp and hindsight.cpp for usage.
        /// </summary>
        struct OptionDescriptor {
            const char* Name;
            const char* Flag;
            const char* Desc;

            /// <summary>
            /// Construct a constexpr new OptionDescriptor.
            /// </summary>
            /// <param name="name">The option or flag lookup name.</param>
            /// <param name="flag">The option or flag notation in CLI (i.e. "-v,--version", "-b", "positional").</param>
            /// <param name="desc">The description, as seen in the --help and --help-all lists.</param>
            constexpr OptionDescriptor(const char* name, const char* flag, const char* desc) 
                : Name(name), Flag(flag), Desc(desc) {

            }
        };

        /// <summary>
        /// DynaCli is a wrapper around CLI11 which can also hold the values for all options, by internally utilizing std::variant.
        /// </summary>
        /// <typeparam name="TTypes">A list of types supported by the created instance of DynaCli.</typeparam>
        /// <remarks>
        /// note: aliases for types can not be used, has i.e. HANDLE and void* are the same; std::variant wants only one occurrence per type.
        ///       You can use DynaCli<bool, void*> and instance.get<HANDLE>(name) for example, as variant will cast identical types.
        /// </remarks>
        template <typename ...TTypes>
        class DynaCli {
            /// <summary>
            /// Bool is required.
            /// </summary>
            static_assert(
                detail::contains_v<bool, TTypes...>,
                "bool is required as a type to be able to work with flags."
            );

            private:
                CLI::App  m_CreatedCommand;
                CLI::App& m_Command;

                std::unordered_map<std::string, detail::InternalOption<TTypes...>> m_Options;
                std::unordered_map<std::string, std::unique_ptr<DynaCli<TTypes...>>> m_Subcommands;
            
                /// <summary>
                /// Throws an exception when <paramref name="name"/> does not exist as option.
                /// </summary>
                /// <param name="name">The option name to assert the existence for.</param>
                void assert_exist(const std::string& name) const {
                    if (!exists(name))
                        throw std::runtime_error("invalid option name, does not exist: " + name); 
                }

                /// <summary>
                /// Throws an exception when <paramref name="name"/> exists as option.
                /// </summary>
                /// <param name="name">The option name to assert the existence for.</param>
                void assert_not_exists(const std::string& name) const {
                    if (exists(name))
                        throw std::runtime_error("invalid option name, already exists: " + name);
                }

                /// <summary>
                /// Throws an exception when the option named <paramref name="name"/> does not hold the type <typeparamref name="TValue"/>.
                /// </summary>
                /// <param name="name">The option name to assert the type for.</param>
                /// <typeparam name="TValue">The required type.</typeparam>
                template<typename TValue>
                void assert_holds_alternative(const std::string& name) const {
                    if (!holds_alternative<TValue>(name))
                        throw std::runtime_error("invalid option type for: " + name);
                }

                /// <summary>
                /// Throws an exception when the subcommand named <paramref name="name"/> does not exist.
                /// </summary>
                /// <param name="name">The subcommand name to assert the existence for.</param>
                void assert_subcommand_exist(const std::string& name) const {
                    if (!subcommand_exist(name))
                        throw std::runtime_error("invalid subcommand: " + name);
                }
            public:
                /// <summary>
                /// Construct a new DynaCli instance from a pointer to a <see cref="::CLI::App"/> instance.
                /// </summary>
                /// <param name="command">The CLI11 command.</param>
                DynaCli(CLI::App* command)
                    : m_Command(*command) {

                }

                /// <summary>
                /// Construct a new DynaCli instance from a reference to a <see cref="::CLI::App"/> instance.
                /// </summary>
                /// <param name="command">The CLI11 command.</param>
                DynaCli(CLI::App& command)
                    : m_Command(command) {

                }

                /// <summary>
                /// Construct a new DynaCli instance, which in turn constructs a new <see cref="::CLI::App"/> instance.
                /// </summary>
                /// <param name="description">The description for the command (or program).</param>
                DynaCli(const std::string& description)
                    : m_CreatedCommand(CLI::App(description)), m_Command(m_CreatedCommand) {

                }

                /// <summary>
                /// Construct a new DynaCli instance, which in turn constructs a new <see cref="::CLI::App"/> instance.
                /// </summary>
                /// <param name="description">The description for the command (or program).</param>
                /// <param name="app_name">The name for the command (or program).</param>
                DynaCli(const std::string& description, const std::string& app_name)
                    : m_CreatedCommand(CLI::App(description, app_name)), m_Command(m_CreatedCommand) {

                }

                /// <summary>
                /// Get a const reference to the internal <see cref="CLI::App"/> instance.
                /// </summary>
                /// <returns>The internal <see cref="CLI::App"/> instance.</returns>
                const CLI::App& command() const {
                    return m_Command;
                }

                /// <summary>
                /// Get a reference to the internal <see cref="CLI::App"/> instance.
                /// </summary>
                /// <returns>The internal <see cref="CLI::App"/> instance.</returns>
                CLI::App& command() {
                    return m_Command;
                }

                /// <summary>
                /// Determines if an option with the name <paramref name="name"/> exists.
                /// </summary>
                /// <param name="name">The name of the option.</param>
                /// <returns>When the option exists, true is returned.</returns>
                bool exists(const std::string& name) const {
                    return m_Options.count(name) != 0;
                }

                /// <summary>
                /// Determines if an option with the name <paramref name="name"/> contains the type <typeparamref name="TValue"/>.
                /// </summary>
                /// <param name="name">The name of the option.</param>
                /// <typeparam name="TValue">The required value.</typeparam>
                /// <returns>When the option <paramref name="name"/> holds a <typeparamref name="TValue"/>, true is returned.</returns>
                template <typename TValue>
                bool holds_alternative(const std::string& name) const {
                    assert_exist(name);

                    return std::holds_alternative<TValue>(m_Options.at(name).Value);
                }

                /// <summary>
                /// Get a const reference to the value of a certain option.
                /// </summary>
                /// <param name="name">The option name.</param>
                /// <returns>A const reference to the value.</returns>
                template <typename TValue>
                const TValue& get(const std::string& name) const {
                    assert_exist(name);
                    assert_holds_alternative<TValue>(name);
                    return std::get<TValue>(m_Options.at(name).Value);
                }

                /// <summary>
                /// Get a reference to the value of a certain option.
                /// </summary>
                /// <param name="name">The option name.</param>
                /// <returns>A reference to the value.</returns>
                template <typename TValue>
                TValue& get(const std::string& name) {
                    assert_exist(name);
                    assert_holds_alternative<TValue>(name);
                    return std::get<TValue>(m_Options.at(name).Value);
                }

                /// <summary>
                /// Set the value of a certain option.
                /// </summary>
                /// <param name="name">The option name.</param>
                /// <param name="value">The value.</param>
                template <typename TValue>
                void set(const std::string& name, const TValue& value) {
                    assert_exist(name);
                    assert_holds_alternative<TValue>(name);
                    std::get<TValue>(m_Options.at(name).Value) = value;
                }

                /// <summary>
                /// Get the value of an option as optional, meaning it can be valueless if 
                /// the option does not exist or does not hold the specified alternative.
                /// </summary>
                /// <param name="name">The option name.</param>
                /// <returns>An optional with or without the option value.</returns>
                template <typename TValue>
                std::optional<TValue> get_optional(const std::string& name) const {
                    if (!exists(name) || !holds_alternative<TValue>(name))
                        return {};
                
                    return std::get<TValue>(m_Options.at(name).Value);
                }

                /// <summary>
                /// Get the value of an option if it was set.
                /// </summary>
                /// <param name="name">The option name.</param>
                /// <returns>An optional wrapping the value or nothing.</returns>
                template <typename TValue>
                std::optional<TValue> get_isset(const std::string& name) const {
                    if (!isset(name))
                        return {};
                    return std::get<TValue>(m_Options.at(name).Value);
                }

                /// <summary>
                /// Get the value of an option if it was set, or a fallback value.
                /// </summary>
                /// <param name="name">The option name.</param>
                /// <param name="def">The fallback value.</param>
                /// <returns>The determined value.</returns>
                template <typename TValue>
                const TValue get_isset_or(const std::string& name, const TValue& def = {}) const {
                    if (!isset(name))
                        return def;
                    return std::get<TValue>(m_Options.at(name).Value);
                }

                /// <summary>
                /// Determine if a certain option was set.
                /// </summary>
                /// <param name="name">The option name.</param>
                /// <returns>True when the option was set, false when it was not set.</returns>
                bool isset(const std::string& name) const {
                    assert_exist(name);
                    return *m_Options.at(name).Option;
                }

                /// <summary>
                /// Determine if any of the options specified in <paramref name="names"/> is set.
                /// </summary>
                /// <param name="names">The initializer list of names.</param>
                /// <returns>True when any of the options is set, false otherwise.</returns>
                bool anyset(const std::initializer_list<std::string>& names) const {
                    for (const auto& name : names) {
                        if (isset(name))
                            return true;
                    }

                    return false;
                }

                /// <summary>
                /// Determine if any of the options specified in <paramref name="names"/> is set in any of the subcommands 
                /// specified in <paramref name="subcommands"/>.
                /// </summary>
                /// <param name="subcommands">The initializer list of subcommand names to check in.</param>
                /// <param name="names">The option names to check for.</param>
                /// <returns>True when any of the options is set, false otherwise.</returns>
                bool subcommand_anyset(const std::initializer_list<std::string>& subcommands, const std::initializer_list<std::string>& names) {
                    for (const auto& subcommand : subcommands) {
                        if (get_subcommand(subcommand).anyset(names))
                            return true;
                    }

                    return false;
                }

                /// <summary>
                /// Add a flag.
                /// </summary>
                /// <param name="name">The option name.</param>
                /// <param name="flag">The short and long flag names.</param>
                /// <param name="desc">The description.</param>
                /// <returns>A new <see cref="CLI::Option"/>.</returns>
                CLI::Option* add_flag(const std::string& name, const std::string& flag, const std::string& desc) {
                    assert_not_exists(name);

                    auto& option  = m_Options[name];
                    option.Value  = false;
                    option.Option = m_Command.add_flag(flag, std::get<bool>(option.Value), desc);
                    return option.Option;
                }

                /// <summary>
                /// Add a flag.
                /// </summary>
                /// <param name="name">The option name.</param>
                /// <param name="flag">The short and long flag names.</param>
                /// <param name="desc">The description.</param>
                /// <param name="func">The function to call when the flag is encountered.</param>
                /// <returns>A new <see cref="CLI::Option"/>.</returns>
                CLI::Option* add_flag(const std::string& name, const std::string& flag, const std::string& desc, std::function<void (size_t)> func) {
                    assert_not_exists(name);

                    auto& option  = m_Options[name];
                    option.Value  = {};
                    option.Option = m_Command.add_flag(flag, func, desc);

                    return option.Option;
                }

                /// <summary>
                /// Add a flag based on an OptionDescriptor.
                /// </summary>
                /// <param name="descriptor">The descriptor for the flag.</param>
                /// <returns>A new <see cref="CLI::Option"/>.</returns>
                CLI::Option* add_flag(const OptionDescriptor& descriptor) {
                    return add_flag(descriptor.Name, descriptor.Flag, descriptor.Desc);
                }

                /// <summary>
                /// Add a flag based on an OptionDescriptor.
                /// </summary>
                /// <param name="descriptor">The descriptor for the flag.</param>
                /// <param name="func">The function to call when the flag is encountered.</param>
                /// <returns>A new <see cref="CLI::Option"/>.</returns>
                CLI::Option* add_flag(const OptionDescriptor& descriptor, std::function<void(size_t)> func) {
                    return add_flag(descriptor.Name, descriptor.Flag, descriptor.Desc, func);
                }

                /// <summary>
                /// Add an option.
                /// </summary>
                /// <typeparam name="TValue">The type of the value for the option.</typeparam>
                /// <param name="name">The option name.</param>
                /// <param name="flag">The short and long flag names.</param>
                /// <param name="desc">The option description.</param>
                /// <returns>A new <see cref="CLI::Option"/>.</returns>
                template <typename TValue>
                CLI::Option* add_option(const std::string& name, const std::string& flag, const std::string& desc) {
                    static_assert(
                        detail::contains_v<TValue, TTypes...>,
                        "invalid type specified in TValue, is not contained in TTypes... of container"
                    );

                    assert_not_exists(name);
                    TValue value  = {};
                    auto& option  = m_Options[name];
                    option.Value  = value;
                    option.Option = m_Command.add_option(flag, std::get<TValue>(option.Value), desc);

                    return option.Option;
                }

                /// <summary>
                /// Add an option based on an OptionDescriptor.
                /// </summary>
                /// <typeparam name="TValue">The type of the value for the option.</typeparam>
                /// <param name="descriptor">The descriptor for the option.</param>
                /// <returns>A new <see cref="CLI::Option"/>.</returns>
                template <typename TValue>
                CLI::Option* add_option(const OptionDescriptor& descriptor) {
                    return add_option<TValue>(descriptor.Name, descriptor.Flag, descriptor.Desc);
                }

                /// <summary>
                /// Get the <see cref="CLI::Option"/> instance for a certain flag.
                /// </summary>
                /// <param name="name">The option name.</param>
                /// <returns>The <see cref="CLI::Option"/>.</returns>
                CLI::Option* get_option(const std::string& name) {
                    assert_exist(name);
                    return m_Options.at(name).Option;
                }

                /// <summary>
                /// Add a subcommand.
                /// </summary>
                /// <param name="name">The subcommand name.</param>
                /// <param name="description">The subcommand description.</param>
                /// <returns>A reference to the new DynaCli instance for this specific type collection.</returns>
                DynaCli<TTypes...>& add_subcommand(const std::string& name, const std::string& description) {
                    auto app = command().add_subcommand(name, description);
                    auto cli = std::make_unique<DynaCli<TTypes...>>(app);
                    m_Subcommands[name] = std::move(cli);
                    return *m_Subcommands[name];
                }

                /// <summary>
                /// Determine if a subcommand exists by name.
                /// </summary>
                /// <param name="name">The subcommand name.</param>
                /// <returns>True when the subcommand exists, false otherwise.</returns>
                bool subcommand_exist(const std::string& name) const {
                    return m_Subcommands.count(name) != 0;
                }

                /// <summary>
                /// Get a reference to an existing subcommand.
                /// </summary>
                /// <param name="name">The subcommand name.</param>
                /// <returns>A reference to the existing subcommand.</returns>
                DynaCli<TTypes...>& get_subcommand(const std::string& name) {
                    assert_subcommand_exist(name);
                    return *m_Subcommands.at(name);
                }

                /// <summary>
                /// Get a const reference to an existing subcommand.
                /// </summary>
                /// <param name="name">The subcommand name.</param>
                /// <returns>A const reference to the existing subcommand.</returns>
                const DynaCli<TTypes...>& get_subcommand(const std::string& name) const {
                    assert_subcommand_exist(name);
                    return *m_Subcommands.at(name);
                }

                /// <summary>
                /// Determine if any subcommand has been chosen.
                /// </summary>
                /// <returns>True when any subcommand was chosen, false otherwise.</returns>
                bool is_subcommand_chosen() const {
                    for (const auto& pair : m_Subcommands) {
                        if (pair.second->command().parsed())
                            return true;
                    }

                    return false;
                }

                /// <summary>
                /// Determine if a specific subcommand has been chosen.
                /// </summary>
                /// <param name="name">The subcommand name to check for.</param>
                /// <returns>True when a specific subcommand was chosen, false otherwise.</returns>
                bool is_subcommand_chosen(const std::string& name) const {
                    assert_subcommand_exist(name);
                    return m_Subcommands.at(name)->command().parsed();
                }

                /// <summary>
                /// Get a const reference to the name of the chosen subcommand.
                /// </summary>
                /// <returns>A const reference to the chosen subcommand name.</returns>
                /// <exception cref="std::runtime_error">Thrown when no subcommand was chosen.</exception>
                const std::string& get_chosen_subcommand_name() const {
                    for (const auto& pair : m_Subcommands) {
                        if (pair.second->command().parsed())
                            return pair.first;
                    }

                    throw std::runtime_error("no subcommand was chosen.");
                }

                /// <summary>
                /// Subscript operator: get a reference to subcommand by name as index.
                /// </summary>
                /// <param name="name">Subcommand name</param>
                /// <returns>A reference to a specific subcommand.</returns>
                DynaCli<TTypes...>& operator[](const std::string& name) {
                    return get_subcommand(name);
                }

                /// <summary>
                /// Subscript operator: get a const reference to subcommand by name as index.
                /// </summary>
                /// <param name="name">Subcommand name</param>
                /// <returns>A const reference to a specific subcommand.</returns>
                const DynaCli<TTypes...>& operator[](const std::string& name) const {
                    return get_subcommand(name);
                }
        };
    }
}

#endif 