auto& operator<<(nl::PcktBuf& mP, const char& mX);

auto& operator>>(nl::PcktBuf& mP, char& mX);

template <typename T>
auto& operator<<(nl::PcktBuf& mP, const std::vector<T>& mX);

template <typename T>
auto& operator>>(nl::PcktBuf& mP, std::vector<T>& mX);

template <typename... Ts>
auto& operator<<(nl::PcktBuf& mP, const ssvu::Tpl<Ts...>& mX);

template <typename... Ts>
auto& operator>>(nl::PcktBuf& mP, ssvu::Tpl<Ts...>& mX);
