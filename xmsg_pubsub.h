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
 * @file xmsg_pubsub.h
 * Copyright (c) 2019, Gaaagaa All rights reserved.
 * 
 * @author  ：Gaaagaa
 * @date    : 2019-11-20
 * @version : 1.0.0.0
 * @brief   : 消息发布订阅模式使用的 订阅者 与 发布者 模板类。
 */

#ifndef __XMSG_PUBSUB_H__
#define __XMSG_PUBSUB_H__

#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <memory>
#include <tuple>
#include <functional>
#include <atomic>
#include <cassert>

////////////////////////////////////////////////////////////////////////////////

#ifndef __X_BUILD_INDEX_SEQUENCE__
#define __X_BUILD_INDEX_SEQUENCE__

//====================================================================

template< size_t... __indexes >
struct xindex_sequence_t
{

};

template< size_t __num, typename __tuple_t = xindex_sequence_t<> >
struct xbuild_index_sequence_t;

template< size_t __num, size_t... __indexes >
struct xbuild_index_sequence_t< __num, xindex_sequence_t< __indexes... > >
    : xbuild_index_sequence_t< __num - 1,
                            xindex_sequence_t< __indexes...,
                                            sizeof...(__indexes) > >
{

};

template< size_t... __indexes >
struct xbuild_index_sequence_t< 0, xindex_sequence_t< __indexes... > >
{
    typedef xindex_sequence_t< __indexes... > type;
};

//====================================================================

#endif // __X_BUILD_INDEX_SEQUENCE__

////////////////////////////////////////////////////////////////////////////////
// xmsg_context_t

/**
 * @struct xmsg_mkey_t< __mkey_t >
 * @brief  用于消息索引键类型声明。
 */
template< typename __mkey_t,
          typename __equal_t = std::equal_to< __mkey_t >,
          typename __hasher_t = std::hash< __mkey_t > >
struct xmsg_mkey_t
{
    typedef typename std::decay< __mkey_t >::type type;
    typedef __equal_t x_equal_t;
    typedef __hasher_t x_hash_t;
};

/**
 * @struct xmsg_args_t< __args_t... >
 * @brief  用于消息参数列表类型声明。
 */
template< typename... __args_t >
struct xmsg_args_t
{
    typedef typename std::tuple<
                typename std::decay< __args_t >::type... > type;
};

/**
 * @class xmsg_context_t< __msg_mkey_t, __msg_args_t... >
 * @brief 发布订阅模式使用的消息模板类。
 * 
 * @param [in ] __msg_mkey_t : 消息索引键值（使用 xmsg_mkey_t 构建）。
 * @param [in ] __msg_args_t : 消息参数列表（使用 xmsg_args_t 构建）。
 */
template< typename __msg_mkey_t, typename __msg_args_t >
class xmsg_context_t
{
    // common data types
public:
    typedef __msg_mkey_t xmsg_mkey_t;
    typedef __msg_args_t xmsg_args_t;

    using x_mkey_t = typename xmsg_mkey_t::type;
    using x_args_t = typename xmsg_args_t::type;

    // constructor/destructor
public:
    explicit xmsg_context_t(void)
        : m_mkey()
        , m_args()
    {

    }

    explicit xmsg_context_t(const x_mkey_t & xmkey, const x_args_t & xargs)
        : m_mkey(xmkey)
        , m_args(xargs)
    {

    }

    explicit xmsg_context_t(const x_mkey_t & xmkey, x_args_t && xargs)
        : m_mkey(xmkey)
        , m_args(std::forward< x_args_t >(xargs))
    {

    }

    template< typename... __vargs_t >
    explicit constexpr xmsg_context_t(const x_mkey_t & xmkey, __vargs_t &&... xargs)
        : m_mkey(xmkey)
        , m_args(std::forward< __vargs_t >(xargs)...)
    {

    }

    xmsg_context_t(xmsg_context_t && xobject) noexcept
    {
        *this = std::forward< xmsg_context_t >(xobject);
    }

    xmsg_context_t & operator = (xmsg_context_t && xobject) noexcept
    {
        if (this == &xobject)
            return *this;

        this->m_mkey = std::move(xobject.m_mkey);
        this->m_args = std::move(xobject.m_args);

        return *this;
    }

    xmsg_context_t(const xmsg_context_t & xobject)
    {
        *this = xobject;
    }

    xmsg_context_t & operator = (const xmsg_context_t & xobject)
    {
        if (this == &xobject)
            return *this;

        this->m_mkey = xobject.m_mkey;
        this->m_args = xobject.m_args;

        return *this;
    }

    ~xmsg_context_t(void)
    {

    }

    // public interfaces
public:
    /**********************************************************/
    /**
     * @brief 消息索引键值。
     */
    inline const x_mkey_t & mkey(void) const { return m_mkey; }

    /**********************************************************/
    /**
     * @brief 消息参数列表。
     */
    inline const x_args_t & args(void) const { return m_args; }

    /**********************************************************/
    /**
     * @brief 消息参数列表。
     */
    inline x_args_t & args(void) { return m_args; }

    // data members
private:
    x_mkey_t  m_mkey;  ///< 消息索引键值
    x_args_t  m_args;  ///< 消息参数列表
};

////////////////////////////////////////////////////////////////////////////////
// xmsg_subscriber_t

typedef size_t   xsub_type_t;

/** 
 * @brief 定义 订阅者基类的类型标识值。
 */
static constexpr xsub_type_t
            XSUBER_BASE_TYPE = (xsub_type_t)(((xsub_type_t)-1) / 2);

/**
 * @class xmsg_subscriber_t< __msg_ctxt_t, __sub_type >
 * @brief 发布订阅模式使用的订阅者模板类。
 * 
 * @param [in ] __sub_type   : 订阅者类型标识。
 * @param [in ] __msg_ctxt_t : 消息类型。
 */
template< xsub_type_t __sub_type, typename __msg_ctxt_t >
class xmsg_subscriber_t;

/**
 * @class xmsg_subscriber_t< __msg_ctxt_t, __sub_type >
 * @brief 发布订阅模式使用的订阅者模板类。
 * 
 * @param [in ] __sub_type   : 订阅者类型标识。
 * @param [in ] __msg_ctxt_t : 消息类型。
 */
template< typename __msg_ctxt_t >
class xmsg_subscriber_t< XSUBER_BASE_TYPE, __msg_ctxt_t >
{
    // common data types
public:
    using x_msgctxt_t = __msg_ctxt_t;

    // constructor/destructor
public:
    xmsg_subscriber_t(void) { }
    virtual ~xmsg_subscriber_t(void) { }

    // extensible interfaces
public:
    /**********************************************************/
    /**
     * @brief 订阅者类型。
     */
    virtual xsub_type_t sub_type(void) const
    {
        return XSUBER_BASE_TYPE;
    }

    /**********************************************************/
    /**
     * @brief 订阅消息的分派接口。
     */
    virtual void translate(const x_msgctxt_t & xmsg_ctxt)
    {
#if 0
        using x_mkey_t = typename x_msgctxt_t::x_mkey_t;
        using x_args_t = typename x_msgctxt_t::x_args_t;
        using x_indices_t =
            typename xbuild_index_sequence_t<
                std::tuple_size< x_args_t >::value >::type;

        invoke_mfunc(xmsg_ctxt.args(), x_indices_t());
#endif
    }

    // public interfaces
public:
#if 0
    /**********************************************************/
    /**
     * @brief 调用消息处理接口。
     */
    template< typename __tuple_t, size_t... __indexes >
    void invoke_mfunc(const __tuple_t & xargs,
                      xindex_sequence_t< __indexes... >)
    {
        // 下面中的 x_subclass_t 为派生的子类。
        using x_mfunc_t = typename void (x_subclass_t::*)(
                            const std::tuple_element< __indexes,
                                                    __tuple_t >::type & ...);

        (static_cast< x_subclass_t * >(this)->*(x_mfunc_t))(
                                            std::get< __indexes >(xargs)...);
    }
#endif
};

/**
 * @class xmsg_subscriber_t< __msg_ctxt_t, __sub_type >
 * @brief 用户自定义（重载）订阅者类 所使用的模板基类。
 * @note
 * 该模板类用于用户自定义（重载）订阅者类。
 * 强制要求（__sub_type < 0x80000000），主要是为了区别于
 * xmsg_publisher_t 内部的 x_subinvoke_t 订阅者类型。
 * 
 * @param [in ] __sub_type   : 订阅者类型标识。
 * @param [in ] __msg_ctxt_t : 消息类型。
 */
template< xsub_type_t __sub_type, typename __msg_ctxt_t >
class xmsg_subscriber_t
    : public xmsg_subscriber_t< XSUBER_BASE_TYPE, __msg_ctxt_t >
{
    static_assert(__sub_type < XSUBER_BASE_TYPE,
                 "__sub_type < XSUBER_BASE_TYPE");

    // common data types
public:
    using x_msgctxt_t = __msg_ctxt_t;

    // constructor/destructor
public:
    xmsg_subscriber_t(void) { }
    virtual ~xmsg_subscriber_t(void) { }

    // extensible interfaces
public:
    /**********************************************************/
    /**
     * @brief 订阅者类型。
     * @note 用户自定义的订阅者类型都 小于 XSUBER_BASE_TYPE 。
     */
    virtual xsub_type_t sub_type(void) const override
    {
        return __sub_type;
    }
};

////////////////////////////////////////////////////////////////////////////////
// xmsg_publisher_t

/**
 * @class xmsg_publisher_t< __msg_context_t, __msg_queue_t >
 * @brief 发布订阅模式使用的订阅者模板类。
 * 
 * @param [in ] __msg_context_t : 消息类型。
 * @param [in ] __msg_queue_t   : 消息队列。
 */
template< typename __msg_context_t,
          typename __msg_queue_t = std::queue< __msg_context_t > >
class xmsg_publisher_t
{
    // common data types
public:
    using x_msgctxt_t    = __msg_context_t;
    using x_msgqueue_t   = __msg_queue_t;
    using x_mkey_t       = typename x_msgctxt_t::x_mkey_t;
    using x_subscriber_t = xmsg_subscriber_t< XSUBER_BASE_TYPE, x_msgctxt_t >;
    using x_subsptr_t    = std::shared_ptr< x_subscriber_t >;
    using x_subwptr_t    = std::weak_ptr< x_subscriber_t >;

    /**
     * @struct x_subkey_t
     * @brief 订阅者对象 在 发布者对象 中的索引键。
     * @note
     * 调用 subscribe() 发起订阅操作时，返回该对象。
     * 其在后续执行中，可用于 unsubscribe() 取消订阅操作。
     */
    struct x_subkey_t
    {
        x_mkey_t    xmkey; ///< 订阅的消息索引键
        x_subwptr_t xwptr; ///< 订阅者对象的弱指针

        // constructor/destructor
    public:
        x_subkey_t(void)
            : xmkey()
            , xwptr()
        {

        }

        x_subkey_t(const x_mkey_t & xmkey, const x_subwptr_t & xwptr)
        {
            this->xmkey = xmkey;
            this->xwptr = xwptr;
        }

        x_subkey_t(const x_subkey_t & xobject)
        {
            *this = xobject;
        }

        x_subkey_t & operator=(const x_subkey_t & xobject)
        {
            if (this != &xobject)
            {
                this->xmkey = xobject.xmkey;
                this->xwptr = xobject.xwptr;
            }

            return *this;
        }

        // public interfaces
    public:
        /**********************************************************/
        /**
         * @brief 判断订阅者对象的订阅状态是否仍然有效。
         */
        inline bool is_valid(void) const { return !xwptr.expired(); }

        /**********************************************************/
        /**
         * @brief 用 bool 操作符判断有效性。
         */
        operator bool (void) const { return is_valid(); }
    };

private:
    /** 用于生成 x_subinvoke_t 类型标识的流水号 */
    static std::atomic_size_t sub_type_seqno;

public:
    /**********************************************************/
    /**
     * @brief 创建一个 x_subinvoke_t 订阅者类的类型标识。
     */
    static xsub_type_t make_sub_type(void)
    {
        return sub_type_seqno.fetch_add(1);
    }

private:
    /**
     * @class x_subinvoke_t< __mfunc_t, __tuple_t >
     * @brief 为了使用 “仿函数对象 与 lambda 表达式 等类函数的泛型接口” 
     * 进行消息处理，而设计的订阅者模板类。
     * 
     * @param [in ] __mfunc_t : 消息处理的回调接口函数。
     * @param [in ] __tuple_t : 回调的参数元组。
     */
    template< typename __mfunc_t, typename __tuple_t >
    class x_subinvoke_t : public x_subscriber_t
    {
        // common data types
    public:
        using x_mfunc_t = __mfunc_t;
        using x_tuple_t = __tuple_t;

    private:
        /** x_subinvoke_t 的订阅者类型 */
        static xsub_type_t sub_type_value;

        // constructor/destructor
    public:
        x_subinvoke_t(x_mfunc_t && xfunc, x_tuple_t && xargs)
            : m_xfunc(std::forward< x_mfunc_t >(xfunc))
            , m_xargs(std::forward< x_tuple_t >(xargs))
        {

        }

        // extensible interfaces
    public:
        /**********************************************************/
        /**
         * @brief 订阅者类型。
         * @note 所有 x_subinvoke_t 的订阅者类型都大于 0x80000000。
         */
        virtual xsub_type_t sub_type(void) const override
        {
            return sub_type_value;
        }

        /**********************************************************/
        /**
         * @brief 订阅消息的分派接口。
         */
        virtual void translate(const x_msgctxt_t & xmsg_ctxt) override
        {
            using x_indices_args_t =
                typename xbuild_index_sequence_t<
                    std::tuple_size<
                        typename x_msgctxt_t::x_args_t >::value >::type;

            using x_indices_invk_t =
                typename xbuild_index_sequence_t<
                    std::tuple_size< x_tuple_t >::value >::type;

            sub_invoke(xmsg_ctxt.args(), x_indices_args_t(), x_indices_invk_t());
        }

        // inner invoking
    private:
        /**********************************************************/
        /**
         * @brief 调用消息处理接口。
         */
        template< typename __args_t,
                  size_t... __indexes_args,
                  size_t... __indexes_invk >
        void sub_invoke(const __args_t & xargs,
                        xindex_sequence_t< __indexes_args... >,
                        xindex_sequence_t< __indexes_invk... >)
        {
#if __cplusplus >= 201703L
            std::invoke(std::forward< __mfunc_t >(m_xfunc),
                        std::get< __indexes_invk >(m_xargs)...,
                        std::get< __indexes_args >(xargs)...);
#else // !(__cplusplus >= 201703L)
            auto xinvoker = std::bind(std::forward< __mfunc_t >(m_xfunc),
                                      std::get< __indexes_invk >(m_xargs)...,
                                      std::get< __indexes_args >(xargs)...);
            xinvoker();
#endif // __cplusplus >= 201703L
        }

        // data members
    private:
        x_mfunc_t  m_xfunc; ///< 消息处理接口
        x_tuple_t  m_xargs; ///< 回调的参数元组
    };

    /**
     * @class x_subptr_t
     * @brief 订阅者对象使用的智能指针类。
     */
    class x_subptr_t : public x_subsptr_t
    {
        // common data types
    public:
        /**
         * @struct x_hash_t
         * @brief 用于计算 哈希键值 的仿函数结构体类型。
         */
        struct x_hash_t
        {
        public:
            typedef x_subptr_t argument_type;
            typedef size_t result_type;

            size_t operator()(const x_subptr_t & xsub_ptr) const
            {
                return m_xfunctor(xsub_ptr.get());
            }

        private:
            std::hash< x_subscriber_t * > m_xfunctor; ///< 仿函数操作实现的对象
        };

        /**
         * @struct x_equal_t
         * @brief 判断相等的仿函数结构体类型。
         */
        struct x_equal_t
        {
        public:
            typedef x_subptr_t first_argument_type;
            typedef x_subptr_t second_argument_type;
            typedef bool result_type;

            bool operator()(const x_subptr_t & xsub_lptr,
                            const x_subptr_t & xsub_rptr) const
            {
                return (xsub_lptr.get() == xsub_rptr.get());
            }
        };

    private:
        using x_super_t = x_subsptr_t;

        /**
         * @struct x_deleter_t< __check_delete >
         * @brief  订阅者对象的删除器。
         */
        template< typename __suber_t, bool __check_delete >
        struct x_deleter_t;

        /**
         * @struct x_deleter_t< true >
         * @brief 订阅者对象的删除器。
         * @note
         * 仅对 x_subinvoke_t 类型的订阅者对象进行删除操作；
         * 非 x_subinvoke_t 类型的订阅者为外部用户自定义的订阅者，
         * 交由外部自行管理。
         */
        template< typename __suber_t >
        struct x_deleter_t< __suber_t, true >
        {
            void operator()(__suber_t * xsub_ptr) const
            {
                if (xsub_ptr->sub_type() > XSUBER_BASE_TYPE)
                    delete xsub_ptr;
            }
        };

        /**
         * @struct x_deleter_t< false >
         * @brief 订阅者对象的删除器。
         * @note 忽略订阅者对象的删除操作。
         */
        template< typename __suber_t >
        struct x_deleter_t< __suber_t, false >
        {
            void operator()(__suber_t * xsub_ptr) const { }
        };

        // constructor/destructor
    public:
        x_subptr_t(x_subscriber_t * xsub_ptr)
            : x_super_t(xsub_ptr, x_deleter_t< x_subscriber_t, true >())
        {

        }

        x_subptr_t(x_subscriber_t * xsub_ptr, bool)
            : x_super_t(xsub_ptr, x_deleter_t< x_subscriber_t, false >())
        {

        }

        // public interfaces
    public:
        /**********************************************************/
        /**
         * @brief 创建 weak_ptr 类型的指针对象。
         */
        inline x_subwptr_t make_weak_ptr(void) const noexcept
        {
            return x_subwptr_t(
                        *static_cast< x_super_t * >(
                            const_cast< x_subptr_t * >(this)));
        }
    };

    using x_subset_t = std::unordered_set<
                                x_subptr_t,
                                typename x_subptr_t::x_hash_t,
                                typename x_subptr_t::x_equal_t >;

    using x_submap_t = std::unordered_map<
                                x_mkey_t,
                                x_subset_t,
                                typename x_msgctxt_t::xmsg_mkey_t::x_hash_t,
                                typename x_msgctxt_t::xmsg_mkey_t::x_equal_t >;

    using x_iterptr_t = std::pair< x_mkey_t, typename x_subset_t::iterator * >;

    // constructor/destructor
public:
    xmsg_publisher_t(void)
    {
        m_iter_suber.first  = x_mkey_t();
        m_iter_suber.second = nullptr;
    }

    ~xmsg_publisher_t(void)
    {

    }

    xmsg_publisher_t(xmsg_publisher_t && xobject) = delete;
    xmsg_publisher_t & operator=(xmsg_publisher_t && xobject) = delete;
    xmsg_publisher_t(const xmsg_publisher_t & xobject) = delete;
    xmsg_publisher_t & operator=(const xmsg_publisher_t & xobject) = delete;

    // public interfaces
public:
    /**********************************************************/
    /**
     * @brief 订阅者对象 订阅指定的消息类型。
     * @note 该接口用于 非x_subinvoke_t（即用户重载的订阅者类） 类型的订阅操作。
     * 
     * @param [in ] xmkey    : 消息类型。
     * @param [in ] xsub_ptr : 订阅者对象。
     * 
     * @return x_subkey_t
     *         - 成功，返回的 x_subkey_t 有效；
     *         - 失败，返回的 x_subkey_t 无效。
     */
    x_subkey_t subscribe(const x_mkey_t & xmkey, x_subscriber_t * xsub_ptr)
    {
        assert(nullptr != xsub_ptr);
        assert(xsub_ptr->sub_type() < XSUBER_BASE_TYPE);

        return iinvk_subscribe(xmkey, xsub_ptr);
    }

    /**********************************************************/
    /**
     * @brief 使用 “类似函数类型” 的订阅者 订阅指定的消息类型。
     * @note 
     * 1. “类似函数类型”，可泛指为 类对象成员函数、仿函数、lambda表达式 等。
     * 2. 可以使用返回的 weak_ptr 所指向的 订阅者对象，
     *    进行 unsubscribe() 取消该消息的订阅操作。
     * 
     * @param [in ] xmkey    : 消息类型。
     * @param [in ] xfunc    : 消息处理的接口函数。
     * @param [in ] xargs... : 回调的参数（或 类对象）。
     * 
     * @return x_subkey_t
     *         - 成功，返回的 x_subkey_t 有效；
     *         - 失败，返回的 x_subkey_t 无效。
     */
    template< typename __mfunc_t, typename... __args_t >
    x_subkey_t subscribe(const x_mkey_t & xmkey,
                         __mfunc_t && xfunc,
                         __args_t &&... xargs)
    {
        using x_mfunc_t = __mfunc_t;
        using x_tuple_t = typename std::tuple<
                                typename std::decay< __args_t >::type... >;

        x_subscriber_t * xsub_ptr =
            new x_subinvoke_t< x_mfunc_t, x_tuple_t >(
                    std::forward< x_mfunc_t >(xfunc),
                    std::forward< x_tuple_t >(
                        x_tuple_t{ std::forward< __args_t >(xargs)... }));
        assert(nullptr != xsub_ptr);

        return iinvk_subscribe(xmkey, xsub_ptr);
    }

    /**********************************************************/
    /**
     * @brief 订阅者对象 取消订阅指定的消息类型。
     * @note
     * 在单线程环境下，若 dispatch() 操作的当前订阅者对象在处理消息时，
     * 调用 unsubscribe() 进行自我取消订阅操作，这是没问题的；
     * 多线程环境下，调用 unsubscribe() 是不安全的操作。
     * 
     * @param [in ] xmkey    : 消息类型。
     * @param [in ] xsub_ptr : 订阅者对象。
     */
    void unsubscribe(const x_mkey_t & xmkey, x_subscriber_t * xsub_ptr)
    {
        typename x_submap_t::iterator itset = m_xmap_suber.find(xmkey);
        if (itset == m_xmap_suber.end())
        {
            return;
        }

        // 若 取消的订阅者 为当前正在投递操作的 订阅者对象，
        // 则需要同步递增 diapatch() 内部操作的 集合迭代器
        if ((nullptr != m_iter_suber.second) &&
            (xmkey == m_iter_suber.first))
        {
            typename x_subset_t::iterator & itsub = *m_iter_suber.second;
            if (xsub_ptr == (*itsub).get())
            {
                itset->second.erase(itsub++);
                m_iter_suber.second = nullptr;

                return;
            }
        }

        itset->second.erase(x_subptr_t(xsub_ptr, false));

        // 若 消息集 为空，则从 消息订阅 的映射表中 删除该 消息集
        if (itset->second.empty())
        {
            // 要取消的消息类型集合，不能为 当前正在投递操作 的集合
            if ((nullptr == m_iter_suber.second) ||
                (xmkey != m_iter_suber.first))
            {
                m_xmap_suber.erase(itset);
            }
        }
    }

    /**********************************************************/
    /**
     * @brief 订阅者对象 取消订阅指定的消息类型。
     * 
     * @param [in ] xsub_key : 其为 subscribe() 返回的订阅者索引键。
     */
    void unsubscribe(const x_subkey_t & xsub_key)
    {
        x_subsptr_t xsub_sptr = xsub_key.xwptr.lock();
        if (nullptr != xsub_sptr)
        {
            unsubscribe(xsub_key.xmkey, xsub_sptr.get());
        }
    }

    /**********************************************************/
    /**
     * @brief 取消 xmkey 下所有的订阅者对象。
     */
    void unsubscribe(const x_mkey_t & xmkey)
    {
        typename x_submap_t::iterator itset = m_xmap_suber.find(xmkey);
        if (itset != m_xmap_suber.end())
        {
            // 要取消的消息类型集合，不能为 当前正在投递操作 的集合
            if ((nullptr == m_iter_suber.second) ||
                (xmkey != m_iter_suber.first))
            {
                m_xmap_suber.erase(itset);
            }
        }
    }

    /**********************************************************/
    /**
     * @brief 返回当前待投递的消息数量。
     */
    inline size_t size(void) const
    {
        return m_xmsg_queue.size();
    }

    /**********************************************************/
    /**
     * @brief 判断消息队列是否为空。
     */
    inline bool empty(void) const
    {
        return (0 == size());
    }

    /**********************************************************/
    /**
     * @brief 发布消息（将消息加入到消息队列）。
     */
    void publish(const x_msgctxt_t & xmsg_ctxt)
    {
        m_xmsg_queue.push(xmsg_ctxt);
    }

    /**********************************************************/
    /**
     * @brief 发布消息（将消息加入到消息队列）。
     */
    void publish(x_msgctxt_t && xmsg_ctxt)
    {
        m_xmsg_queue.push(std::forward< x_msgctxt_t >(xmsg_ctxt));
    }

    /**********************************************************/
    /**
     * @brief 发布消息（将消息加入到消息队列）。
     */
    template< typename... __args_t >
    void publish(const x_mkey_t & xmkey, __args_t &&... xargs)
    {
        static_assert(
            std::tuple_size<
                    typename x_msgctxt_t::x_args_t
                >::value == sizeof...(xargs),
            "Incorrect the number of arguments!");

        publish(
            std::forward< x_msgctxt_t >(
                x_msgctxt_t(xmkey, std::forward< __args_t >(xargs)...)));
    }

    /**********************************************************/
    /**
     * @brief 投递消息，结果返回投递的消息数量。
     */
    size_t dispatch(size_t xmsg_maxcount = (size_t)-1)
    {
        size_t xmsg_count = 0;
        x_msgctxt_t xmsg_ctxt;

        typename x_submap_t::iterator itset;
        typename x_subset_t::iterator itsub;

        while (!empty() && (xmsg_maxcount-- > 0))
        {
            xmsg_ctxt = std::move(m_xmsg_queue.front());
            m_xmsg_queue.pop();

            itset = m_xmap_suber.find(xmsg_ctxt.mkey());
            if (itset != m_xmap_suber.end())
            {
                m_iter_suber.first = xmsg_ctxt.mkey();

                for (itsub = itset->second.begin();
                     itsub != itset->second.end();)
                {
                    m_iter_suber.second = &itsub;
                    (*itsub)->translate(xmsg_ctxt);

                    if (nullptr != m_iter_suber.second)
                    {
                        ++itsub;
                        m_iter_suber.second = nullptr;
                    }
                }

                m_iter_suber.first = x_mkey_t();

                if (itset->second.empty())
                {
                    m_xmap_suber.erase(itset);
                }
            }

            xmsg_count += 1;
        }

        return xmsg_count;
    }

    // inner invoking
private:
    /**********************************************************/
    /**
     * @brief 订阅者对象 订阅指定的消息类型。
     * 
     * @param [in ] xmkey    : 消息类型。
     * @param [in ] xsub_ptr : 订阅者对象。
     * 
     * @return x_subkey_t
     *         - 成功，返回的 x_subkey_t 有效；
     *         - 失败，返回的 x_subkey_t 无效。
     */
    x_subkey_t iinvk_subscribe(const x_mkey_t & xmkey,
                               x_subscriber_t * xsub_ptr)
    {
        using x_ipair_t = typename std::pair<
                                typename x_subset_t::iterator, bool >;

        x_subptr_t xsub_optr(xsub_ptr);
        x_subset_t & xsub_set = m_xmap_suber[xmkey];

        x_ipair_t xipair = xsub_set.insert(xsub_optr);
        if (xipair.second)
        {
            return x_subkey_t(xmkey, xsub_optr.make_weak_ptr());
        }

        if (xsub_set.empty())
        {
            m_xmap_suber.erase(xmkey);
        }

        return x_subkey_t();
    }

    // data members
private:
    x_iterptr_t   m_iter_suber;  ///< 指向当前正在进行消息投递操作的订阅者信息
    x_submap_t    m_xmap_suber;  ///< 消息订阅者映射表
    x_msgqueue_t  m_xmsg_queue;  ///< 消息队列
};

/** 用于生成 x_subinvoke_t 类型标识的流水号 */
template< typename __msg_context_t, typename __msg_queue_t >
std::atomic_size_t xmsg_publisher_t<
                        __msg_context_t,
                        __msg_queue_t
                   >::sub_type_seqno(XSUBER_BASE_TYPE + 1);

/** x_subinvoke_t 的订阅者类型 */
template< typename __msg_context_t, typename __msg_queue_t >
template< typename __mfunc_t, typename __tuple_t >
xsub_type_t xmsg_publisher_t<
                        __msg_context_t,
                        __msg_queue_t
                   >::x_subinvoke_t<
                        __mfunc_t,
                        __tuple_t >::sub_type_value =
            xmsg_publisher_t<
                        __msg_context_t,
                        __msg_queue_t >::make_sub_type();

////////////////////////////////////////////////////////////////////////////////

#endif // __XMSG_PUBSUB_H__
