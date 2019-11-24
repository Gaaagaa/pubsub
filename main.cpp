/**
 * @file    main.cpp
 * <pre>
 * Copyright (c) 2019, Gaaagaa All rights reserved.
 * 
 * 文件名称：main.cpp
 * 创建日期：2019年11月20日
 * 文件标识：
 * 文件摘要：发布订阅模式的测试程序。
 * 
 * 当前版本：1.0.0.0
 * 作    者：
 * 完成日期：2019年11月20日
 * 版本摘要：
 * 
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 * 版本摘要：
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
