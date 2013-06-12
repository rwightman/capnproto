// Copyright (c) 2013, Kenton Varda <temporal@gmail.com>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef KJ_VECTOR_H_
#define KJ_VECTOR_H_

#include "array.h"

namespace kj {

template <typename T>
class Vector {
  // Similar to std::vector, but based on KJ framework.
  //
  // This implementation always uses move constructors when growing the backing array.  If the
  // move constructor throws, the Vector is left in an inconsistent state.  This is acceptable
  // under KJ exception theory which assumes that exceptions leave things in inconsistent states.

public:
  inline Vector() = default;
  inline explicit Vector(size_t capacity): builder(heapArrayBuilder<T>(capacity)) {}

  inline operator ArrayPtr<T>() { return builder; }
  inline operator ArrayPtr<const T>() const { return builder; }
  inline ArrayPtr<T> asPtr() { return builder.asPtr(); }
  inline ArrayPtr<const T> asPtr() const { return builder.asPtr(); }

  inline size_t size() const { return builder.size(); }
  inline bool empty() const { return size() == 0; }
  inline size_t capacity() const { return builder.capacity(); }
  inline T& operator[](size_t index) const { return builder[index]; }

  inline const T* begin() const { return builder.begin(); }
  inline const T* end() const { return builder.end(); }
  inline const T& front() const { return builder.front(); }
  inline const T& back() const { return builder.back(); }
  inline T* begin() { return builder.begin(); }
  inline T* end() { return builder.end(); }
  inline T& front() { return builder.front(); }
  inline T& back() { return builder.back(); }

  template <typename... Params>
  inline void add(Params&&... params) {
    if (builder.isFull()) grow();
    builder.add(kj::fwd<Params>(params)...);
  }

private:
  ArrayBuilder<T> builder;

  void grow() {
    size_t newSize = capacity() == 0 ? 4 : capacity() * 2;
    ArrayBuilder<T> newBuilder = heapArrayBuilder<T>(newSize);
    for (T& element: builder) {
      newBuilder.add(kj::mv(element));
    }
    builder = kj::mv(newBuilder);
  }
};

}  // namespace kj

#endif  // KJ_VECTOR_H_
