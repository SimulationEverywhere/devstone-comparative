#ifndef CADMIUM_DEVSTONE_ATOMIC_HPP
#define CADMIUM_DEVSTONE_ATOMIC_HPP

#include<cadmium/modeling/ports.hpp>
#include<cadmium/modeling/message_bag.hpp>
#include<limits>

/**
 * @brief DEVStone atomic model state.
 * @tparam TIME data type used to represent the time.
 */
template<typename TIME>
struct devstone_state {
    TIME sigma;
    unsigned int n_internals, n_externals, n_events;
    devstone_state(): sigma(std::numeric_limits<TIME>::infinity()), n_internals(), n_externals(), n_events() {}

    friend std::ostream &operator << (std::ostream &os, const devstone_state<TIME> &s) {
        os << s.n_internals << " ; " << s.n_externals << " ; " << s.n_events;
        return os;
    }
};

//  an integer input and output port for the model
struct devstone_atomic_defs{
    //custom ports
    struct in : public cadmium::in_port<int> {};
    struct out : public cadmium::out_port<int> {};
};

template<typename TIME>
struct devstone_atomic {
    using defs=devstone_atomic_defs;
    using input_ports=std::tuple<typename defs::in>;
    using output_ports=std::tuple<typename defs::out>;
    using outbag_t=typename cadmium::make_message_bags<output_ports>::type;
    using state_type=devstone_state<TIME>;

    state_type state = devstone_state<TIME>();
    [[maybe_unused]] int internal_cycles, external_cycles;
    outbag_t outbag;

    constexpr devstone_atomic() noexcept: internal_cycles(), external_cycles() {
        //preparing the output bag, since we return always same message
        cadmium::get_messages<typename defs::out>(outbag).emplace_back(1);
    }

    constexpr devstone_atomic(int int_cycles, int ext_cycles) noexcept: internal_cycles(int_cycles), external_cycles(ext_cycles) {
        //preparing the output bag, since we return always same message
        cadmium::get_messages<typename defs::out>(outbag).emplace_back(1);
    }

public:
    void internal_transition() {
        // DhryStone().dhrystoneRun(internal_cycles);  // TODO enable Dhrystone cycles
        state.sigma = std::numeric_limits<TIME>::infinity();
        state.n_internals += 1;
    }

    void external_transition(TIME e, typename cadmium::make_message_bags<input_ports>::type mbs) {
        // DhryStone().dhrystoneRun(external_cycles);  // TODO enable Dhrystone cycles
        state.sigma = TIME();
        state.n_externals += 1;
        state.n_events += cadmium::get_messages<typename defs::in>(mbs).size();
    }

    [[maybe_unused]] void confluence_transition(TIME e, typename cadmium::make_message_bags<input_ports>::type mbs) {
        internal_transition();
        external_transition(e, mbs);
    }

    [[maybe_unused]][[nodiscard]] outbag_t output() const {
        return outbag;
    }

    [[maybe_unused]] TIME time_advance() const {
        return state.sigma;
    }
};

#endif // CADMIUM_DEVSTONE_ATOMIC_HPP
