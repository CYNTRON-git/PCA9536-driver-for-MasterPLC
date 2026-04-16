#pragma once
#include <share/config.h>
#include <mplc/libs/threads.hpp>

namespace mplc { namespace ws {

    class ts_buffer final {
    public:
        struct small_buf_t {
            char buf[1024];
            ws::ts_buffer* out_buf{};
            uint32_t pos{0};
            lib::atomic_flag buf_lock;
            // ReSharper disable once CppPossiblyUninitializedMember
            small_buf_t(ws::ts_buffer& out_buf): out_buf(&out_buf) {}
            small_buf_t(small_buf_t&& rv) noexcept: out_buf(rv.out_buf) {
                rv.flush();
                if (rv.buf_lock.test_and_set()) {
                    buf_lock.test_and_set();
                }
                rv.out_buf = nullptr;
            }
            void write(const char* in, uint32_t size) {
                if (size < sizeof(buf) - pos) {
                    memcpy(&buf[pos], in, size);
                    pos += size;
                } else {
                    flush();
                    out_buf->write(in, size);
                }
            }
            void flush() {
                if (pos && out_buf) {
                    if (!buf_lock.test_and_set()) {
                        out_buf->m_stream_mtx.lock();
                    }
                    out_buf->write(buf, pos);
                    pos = 0;
                }
            }
            ~small_buf_t() {
                flush();
                if (out_buf && buf_lock.test_and_set()) {
                    out_buf->m_stream_mtx.unlock();
                }
            }
        };

        //! Chunk header for perpending to each chunk.
        /*! Chunks are stored as a singly linked list.
         */
        struct chunk_t;
        ts_buffer& operator=(ts_buffer&& rhs) noexcept;
        //! Copy constructor is not permitted.
        ts_buffer(const ts_buffer& rhs) = delete;
        //! Copy assignment operator is not permitted.
        ts_buffer& operator=(const ts_buffer& rhs) = delete;
        ts_buffer(ts_buffer&& rhs) noexcept;

        ts_buffer(uint32_t chunkSize = kDefaultChunkCapacity) noexcept;

        ~ts_buffer() noexcept;
        //! Data alignment of the machine.

        //! Thread safe deallocates all memory chunks, excluding the user-supplied buffer.
        void clear() noexcept;

        bool empty() const noexcept;

        // Thread safe for single writer
        void write(const char* buf, uint32_t len) noexcept;
        // Thread safe
        ts_buffer flush();

        chunk_t* head() {
            return m_head;
        }
        chunk_t* cut_head();
        // Return begin chunk buffer
        static unsigned char* chunk_data(chunk_t* chunk);
        // Return avalible space from chunk
        static unsigned char* chunk_buf(chunk_t* chunk);

        // Return avalible data from chunk
        static size_t chunk_size(chunk_t* chunk);

        static void chunk_free(chunk_t* chunk);

        static chunk_t* next_chunk(chunk_t* chunk);

        //! NOT thread safe
        void reverse();
        small_buf_t get_stream() {
            return small_buf_t(*this);
        }

    private:
        /*
            Some machines require strict data alignment. The default is 8 bytes.
            User can customize by defining the RAPIDJSON_ALIGN function macro.
        */
        static size_t alingof(size_t size) noexcept;

        //! Creates a new chunk.
        /*! \param capacity Capacity of the chunk in bytes.
            \return nullptr if failed
        */
        static chunk_t* create_chunk(uint32_t capacity) noexcept;

        //! Frees a memory block (concept Allocator)
        static void dealocate(void* ptr) noexcept;

        static const int kDefaultChunkCapacity = 16 * 1024;
        chunk_t* m_head;            //!< Head of the chunk linked-list. Only the
                                    //!< head chunk serves allocation.
        uint32_t m_chunk_capacity;  //!< The minimum capacity of chunk when they are allocated.
        mutable lib::spinlock m_spin;
        mutable lib::mutex m_stream_mtx;
    };

}}  // namespace mplc::ws
