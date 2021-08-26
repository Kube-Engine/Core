/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: String literal
 */

template<typename ...Args>
inline std::string kF::Literal::FormatStdString(Args &&...args)
{
    constexpr auto Concat = [](std::string &res, const auto &arg) {
        using ArgType = std::remove_cvref_t<decltype(arg)>;
        if constexpr ((!std::is_integral_v<ArgType> || std::is_same_v<ArgType, char>)
                && Core::Utils::IsDetectedExact<std::string &, Internal::StringConcatable, decltype(arg)>)
            res += arg;
        else
            res += std::to_string(arg);
    };

    std::string res;
    (Concat(res, args), ...);
    return res;
}