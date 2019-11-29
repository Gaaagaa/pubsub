/**
 * The MIT License (MIT)
 * Copyright (c) 2019, Gaaagaa All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * @file main.cpp
 * <pre>
 * Copyright (c) 2019, Gaaagaa All rights reserved.
 * 
 * author ：Gaaagaa
 * date   : 2019-11-29
 * info   : 发布订阅模式的测试程序。
 * </pre>
 */

#include "xmsg_pubsub.h"

#include <string>
#include <sstream>
#include <iostream>
#include <thread>

////////////////////////////////////////////////////////////////////////////////

using xmsg_ctxt_t = xmsg_context_t<
                        xmsg_mkey_t< int >,
                        xmsg_args_t< std::string > >;
using xmsg_pub_t  = xmsg_publisher_t< xmsg_ctxt_t >;
using x_subkey_t = xmsg_pub_t::x_subkey_t;

const int XSUB_MKEY_TEST = 100;

xmsg_pub_t xpub;
x_subkey_t xfunc_skey;
x_subkey_t xfunx_skey;

void sub_func(const std::string & str)
{
    std::cout << "sub_func: " << str << std::endl;

    xpub.unsubscribe(xfunx_skey);
}

void sub_funx(const std::string & str)
{
    std::cout << "sub_funx: " << str << std::endl;
}

struct suber_t
{
public:
    void sub_ivk(const std::string & str)
    {
        std::cout << "sub_ivk : " << str << std::endl;

        xpub.unsubscribe(m_ivk_skey);
    }

    void sub_ext(const std::string & str)
    {
        std::cout << "sub_ext : " << str << std::endl;

        xpub.unsubscribe(m_ext_skey);
    }

public:
    x_subkey_t m_ivk_skey;
    x_subkey_t m_ext_skey;
};

int main(int argc, char * argv[])
{
    xfunc_skey = xpub.subscribe(XSUB_MKEY_TEST, &sub_func);
    xfunx_skey = xpub.subscribe(XSUB_MKEY_TEST, &sub_funx);

    suber_t xsub;
    xsub.m_ivk_skey = xpub.subscribe(XSUB_MKEY_TEST, &suber_t::sub_ivk, &xsub);
    xsub.m_ext_skey = xpub.subscribe(XSUB_MKEY_TEST, &suber_t::sub_ext, &xsub);

    x_subkey_t xskey_lambda1 =
        xpub.subscribe(XSUB_MKEY_TEST,
            [](const std::string & xstr) -> void
            {
                std::cout << "lambda1 : " << xstr << std::endl;
            });

    x_subkey_t xskey_lambda2 =
        xpub.subscribe(XSUB_MKEY_TEST,
            [](const std::string & xstr) -> void
            {
                std::cout << "lambda2 : " << xstr << std::endl;
            });

    std::cout << "sub type nums: " << xpub.make_sub_type() - XSUBER_BASE_TYPE - 1 << std::endl;

#define XSUB_TYPE(xskey)                       \
    do                                         \
    {                                          \
        xmsg_pub_t::x_subsptr_t xsub_sptr =    \
                        xskey.xwptr.lock();    \
        if (nullptr != xsub_sptr)              \
            std::cout << __LINE__              \
                      << " type: "             \
                      << xsub_sptr->sub_type() \
                      << std::endl;            \
    } while (0)

    XSUB_TYPE(xfunc_skey);
    XSUB_TYPE(xfunx_skey);
    XSUB_TYPE(xsub.m_ivk_skey);
    XSUB_TYPE(xsub.m_ext_skey);
    XSUB_TYPE(xskey_lambda1);
    XSUB_TYPE(xskey_lambda2);

    size_t xmsg_id = 0;

    while (true)
    {
        for (size_t i = 0; i < 4; ++i)
        {
            std::ostringstream ostr;
            ostr << "xmsg -> " << xmsg_id++;

            xpub.publish(XSUB_MKEY_TEST, ostr.str());
        }

        xpub.dispatch();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}
