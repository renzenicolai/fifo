//  SPDX-FileCopyrightText: 2021 Renze Nicolai
//  SPDX-License-Identifier: MIT

#include <atomic>
#include <stdexcept>
#include <cstring>

template <typename Type> class Fifo {
    public:
        Fifo(size_t aBufferSize) :
            mBufferSize(aBufferSize) {
                mpBuffer = new Type[mBufferSize];
                mReadPosition.store(0);
                mWritePosition.store(0);
        }

        ~Fifo() {
            delete mpBuffer;
        }

        Type* pop() {
            size_t currentReadPosition = mReadPosition.load();
            size_t currentWritePosition = mWritePosition.load();
            if (currentReadPosition == currentWritePosition) {
                throw std::underflow_error("queue is empty");
            }
            size_t nextReadPosition = (currentReadPosition + 1) % mBufferSize;
            mReadPosition.store(nextReadPosition);
            return &mpBuffer[currentReadPosition];
        }

        void push(const Type* aObject) {
            size_t currentReadPosition = mReadPosition.load();
            size_t currentWritePosition = mWritePosition.load();
            size_t nextWritePosition = (currentWritePosition + 1) % mBufferSize;
            if (nextWritePosition == currentReadPosition) {
                throw std::overflow_error("queue is full");
            }
            Type* target = &mpBuffer[currentWritePosition];
            memcpy(target, aObject, sizeof(Type));
            mWritePosition.store(nextWritePosition);
        }

    private:
        Type* mpBuffer;
        size_t mBufferSize;
        std::atomic<size_t> mReadPosition;
        std::atomic<size_t> mWritePosition;
};