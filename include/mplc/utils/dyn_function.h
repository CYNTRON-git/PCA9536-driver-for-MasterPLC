#pragma once
// #include <functional>
#include <boost/dll.hpp>
#include "mplc/log.h"
#ifdef _WIN32
#    include <boost/winapi/get_proc_address.hpp>
#else
#    include <dlfcn.h>
#endif

namespace mplc::utils {
    /**
     *  Function.h
     *
     *  When you want to call a function only if it is already linked into
     *  the program space. you would normally use the dlsym() function for
     *  that. Th Function object in this file is a little more convenient:
     *
     *      // get the function object
     *      Function func<int(int)> func("example_function");
     *
     *      // call the function
     *      int result = func(123);
     *
     *  @author Emiel Bruijntjes <emiel.bruijntjes@copernica.com>
     *  @copyright 2018 Copernica BV
     */

    /**
     *  Make the Function class a templated class
     */
    template<class T>
    class dyn_function {};

    /**
     *  So that we can write a partial specialisation
     *  using a function prototype, indicating the
     *  prototype usable for the given callback
     */
    template<class T, class... Arguments>
    class dyn_function<T(Arguments...)> {
    private:
        /**
         *  Store pointer to the actual dynamically loaded
         *  function. This is done in a union because the
         *  result from a dlsym call is a void pointer which
         *  cannot be legally cast into a function pointer.
         *
         *  We therefore always set the first type (which is
         *  void* making it legal) and, because all members
         *  in a union share the same address, we can then
         *  read the second type and actually call it.
         *
         *  @var Callable
         */
        using function_t = T (*)(Arguments...);
        union Callable {
            /**
             *  Property for getting and setting the return
             *  value from dlsym. This is always a void*
             *  @var    void
             */
            void* ptr;

            /**
             *  Property for executing the mapped function
             *
             *  @param  mixed,...   function parameters
             *  @return mixed
             *
             *  @var    function
             */
            // T (*func)(Arguments...);
            function_t func;
            /**
             *  Constructor
             */
            Callable(): ptr(nullptr) {}

            /**
             *  We may be moved
             *
             *  @param  callable    the callable we are moving
             */
            Callable(Callable&& callable): ptr(callable.ptr) {
                // the other callable no longer has a handle
                callable.ptr = nullptr;
            }

            /**
             *  Copy construtor
             *  @param  callable    the callable we are moving
             */
            Callable(const Callable& callable): ptr(callable.ptr) {}

            /**
             *  Constructor
             *
             *  @param  function    the mapped function
             */
            Callable(void* function): ptr(function) {}

        } _method;

    public:
        /**
         *  Constructor
         *  @param  handle      Handle to access openssl
         *  @param  name        Name of the function
         */
        dyn_function(void* handle, const char* name) {
#ifdef _WIN32
            _method.ptr = GetProcAddress((HMODULE)handle, name);
#else
            _method.ptr = dlsym(handle, name);
#endif
            if (!_method.ptr) {
                PRINT_ERROR("Can't load function: '%s' ", name);
                // throw std::runtime_error(dlerror());
            }
        }
        dyn_function(const boost::dll::shared_library& lib, const char* name) {
            boost::dll::fs::error_code ec;
            if (lib.has(name)) {
                _method.func = lib.get<function_t>(name);
            } else {
                PRINT_ERROR("Can't load function: '%s' from '%s'", name, lib.location(ec).string().c_str());
            }
        }
        /**
         *  Destructor
         */
        virtual ~dyn_function() {}

        /**
         *  Is this a valid function or not?
         *  @return bool
         */
        bool valid() const {
            return _method.ptr != nullptr;
        }

        /**
         *  The library object can also be used as in a boolean context,
         *  for that there is an implementation of a casting operator, and
         *  the negate operator
         *  @return bool
         */
        operator bool() const {
            return valid();
        }
        bool operator!() const {
            return !valid();
        }

        /**
         *  Test whether we are a valid object
         *  @param  nullptr test if we are null
         */
        bool operator==(std::nullptr_t /* nullptr */) const {
            return !valid();
        }
        bool operator!=(std::nullptr_t /* nullptr */) const {
            return valid();
        }

        /**
         *  Invoke the function
         *
         *  @param  mixed,...
         */
        T operator()(Arguments... parameters) const {
            // check whether we have a valid function
            if (!valid())
                throw std::bad_function_call();

            // execute the method given all the parameters
            return _method.func(std::forward<Arguments>(parameters)...);
        }
    };
}  // namespace mplc::utils
