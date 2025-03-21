// IOUringHandler.h
#pragma once

#include "Buffer.h"
#include <liburing.h>
#include <memory>
#include <string>
#include <unordered_map>

class IOUringHandler {
public:
  IOUringHandler(int entries = 256);
  ~IOUringHandler();

  // 禁止拷贝和移动
  IOUringHandler(const IOUringHandler &) = delete;
  IOUringHandler &operator=(const IOUringHandler &) = delete;

  // 异步读取
  bool readAsync(int fd, Buffer &buffer, Buffer::ReadCallback cb);

  // 异步写入
  bool writeAsync(int fd, Buffer &buffer, Buffer::WriteCallback cb);

  // 处理完成事件
  void poll(int timeout_ms = -1);

private:
  struct Operation {
    enum Type { READ, WRITE } type;
    int fd;
    Buffer *buffer;
    union {
      Buffer::ReadCallback readCb;
      Buffer::WriteCallback writeCb;
    };

    Operation(Type t, int f, Buffer *b) : type(t), fd(f), buffer(b) {}

    ~Operation() {
      if (type == READ) {
        // readCb.~ReadCallback();
      } else {
        // writeCb.~WriteCallback();
      }
    }
  };

  struct io_uring ring;
  std::unordered_map<__u64, std::unique_ptr<Operation>> operations;
  __u64 next_id = 1;
};