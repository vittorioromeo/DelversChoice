template <typename... Ts>
void serialize(PcktBuf& p, Ts&&... xs);

template <typename... Ts>
void deserialize(PcktBuf& p, Ts&... xs);

template <typename... Ts>
auto make_serialized(Ts&&... xs);

template <typename T>
auto make_deserialized(PcktBuf& p);