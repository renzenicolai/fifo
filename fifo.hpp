//  SPDX-FileCopyrightText: 2021 Renze Nicolai
//  SPDX-License-Identifier: MIT

#include <atomic>
#include <stdexcept>
#include <cstring>
#include <semaphore.h>

template <typename Type> class Fifo {
    public:
        Fifo(size_t buffer_size) :
            buffer_size_(buffer_size),
            semaphore_(nullptr) {
                buffer_ = new Type[buffer_size_];
                read_position_.store(0);
                write_position_.store(0);
        }

        ~Fifo() {
            delete buffer_;
        }

        /* Reader */

        Type* pop() {
            size_t current_read_position = read_position_.load();
            size_t current_write_position = write_position_.load();
            if (current_read_position == current_write_position) {
                throw std::underflow_error("queue is empty");
            }
            size_t nextReadPosition = (current_read_position + 1) % buffer_size_;
            read_position_.store(nextReadPosition);
            return &buffer_[current_read_position];
        }

        Type* peek() {
            size_t current_read_position = read_position_.load();
            size_t current_write_position = write_position_.load();
            if (current_read_position == current_write_position) {
                throw std::underflow_error("queue is empty");
            }
            return &buffer_[current_read_position];
        }

        /* Writer */

        void push(const Type* object) {
            size_t current_read_position = read_position_.load();
            size_t current_write_position = write_position_.load();
            size_t nextWritePosition = (current_write_position + 1) % buffer_size_;
            if (nextWritePosition == current_read_position) {
                throw std::overflow_error("queue is full");
            }
            Type* target = &buffer_[current_write_position];
            memcpy(target, object, sizeof(Type));
            write_position_.store(nextWritePosition);
            if (semaphore_ != nullptr) {
                sem_post(semaphore_);
            }
        }

        /* Statistics */

        size_t size() {
            return buffer_size_ - 1;
        }

        size_t used() {
            size_t read_position = read_position_.load();
            size_t write_position = write_position_.load();
            return (write_position - read_position) % buffer_size_;
        }

        size_t free() {
            return size() - used();
        }

        /* Semaphore */

        void set_semaphore(sem_t* semaphore) {
            semaphore_ = semaphore;
        }

    private:
        Type* buffer_; /// The ringbuffer storing the contents of the fifo
        size_t buffer_size_; /// The amount of objects that can fit in the ringbuffer
        std::atomic<size_t> read_position_; /// Position in the buffer at which the reader thread is reading
        std::atomic<size_t> write_position_; /// Position in the buffer at which the writer thread is writing
        sem_t* semaphore_; // Pointer to the semaphore object to post to when push is called (or nullptr when not used)
};