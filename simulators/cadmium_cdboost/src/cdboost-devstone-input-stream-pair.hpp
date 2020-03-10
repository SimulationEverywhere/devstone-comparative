/**
 * Copyright (c) 2013-2014, Carleton University, Universite de Nice-Sophia Antipolis
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef P_DEVSTONE_INPUT_STREAM_PAIR_H
#define P_DEVSTONE_INPUT_STREAM_PAIR_H

#include <istream>
#include <sstream>
#include <boost/simulation/pdevs/atomic.hpp>

using namespace boost::simulation::pdevs;

namespace cdpp {
/**
 * @brief input_stream PDEVS Model.
 *
 * istream PDEVS Model plays a history of events received by an input stream.
 * The list of events allows to be used as a connector to external tools.
 * The input format is "time output and a custom parser can be defined."
 *
*/
template<class TIME, class MSG>
class devstone_input_stream_pair : public boost::simulation::pdevs::atomic<TIME, MSG>
{
    std::vector<MSG> _out;
    TIME _period;
    int _state;  // 0 passive, 1 active
public:
    /**
     * @brief DEVStoneAtomic constructor.
     *
     * @param internal_cycles the cycles dhrystone will be run in internal transitions.
     * @param external_cycles the cycles dhrystone will be run in external transitions.
     * @param period the time used for all time_advances.
     */
    explicit devstone_input_stream_pair(TIME period)
            : _period(period)
    {
        _state = 1;
        _out.push_back(std::make_pair(0, 0));
        _out.push_back(std::make_pair(1, 0));
    }
    /**
     * @brief internal function.
     */
    void internal() noexcept {
        _state = 0;
    }
    /**
     * @brief advance function.
     * @return Time until next internal event.
     */
    TIME advance() const noexcept {
        return ((_state==1)? 0 : boost::simulation::pdevs::atomic<TIME, MSG>::infinity);
    }
    /**
     * @brief out function.
     * @return Message defined in contruction.
     */
    std::vector<MSG> out() const noexcept{
        return _out;
    }
    /**
     * @brief external function domain is empty, so it throws.
     * @param msg external input message.
     * @param t time the external input is received.
     */
    void external(const std::vector<MSG>& msg, const TIME& t) noexcept {

    }
    /**
     * @brief confluence function as defined in PDEVS
     * Message interpretation (with cdpp::message_cast) is confluence function responsability.
     * @param mb is a bag of messages coming from outside
     * @param t is the time the message is received
     */
    void confluence(const std::vector<MSG>& mb, const TIME& t) noexcept{
        internal();
        external(mb, TIME(0));
    }

};

}

#endif // P_DEVSTONE_INPUT_STREAM_PAIR_H
