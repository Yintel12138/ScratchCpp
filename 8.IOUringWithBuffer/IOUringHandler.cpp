// IOUringHandler.cpp
#include "IOUringHandler.h"
#include <cstring>
#include <iostream>
#include <sys/eventfd.h>

IOUringHandler::IOUringHandler(int entries) {
  struct io_uring_params params;
  memset(&params, 0, sizeof(params));

  int ret = io_uring_queue_init_params(entries, &ring, &params);
  if (ret < 0) {
    throw std::runtime_error("Failed to initialize io_uring: " +
                             std::string(strerror(-ret)));
  }
}

IOUringHandler::~IOUringHandler() { io_uring_queue_exit(&ring); }

bool IOUringHandler::readAsync(int fd, Buffer &buffer,
                               Buffer::ReadCallback cb) {
  // 获取一个SQE
  struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
  if (!sqe) {
    std::cerr << "Failed to get SQE for read operation" << std::endl;
    return false;
  }

  // 创建操作对象
  __u64 id = next_id++;
  auto op = std::make_unique<Operation>(Operation::READ, fd, &buffer);
  op->readCb = std::move(cb);
  
  // 准备读取操作
  void *buf = buffer.beginWrite();
  size_t len = buffer.writableBytes();
  io_uring_prep_read(sqe, fd, buf, len, 0); // 从offset 0开始读取
  io_uring_sqe_set_data(sqe, reinterpret_cast<void *>(id));

  // 保存操作对象
  operations[id] = std::move(op);

  return true;
}

bool IOUringHandler::writeAsync(int fd, Buffer &buffer,
                                Buffer::WriteCallback cb) {
  // 获取一个SQE
  struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
  if (!sqe) {
    std::cerr << "Failed to get SQE for write operation" << std::endl;
    return false;
  }

  // 创建操作对象
  __u64 id = next_id++;
  auto op = std::make_unique<Operation>(Operation::WRITE, fd, &buffer);
  op->writeCb = std::move(cb);

  // 准备写入操作
  const void *buf = buffer.peek();
  size_t len = buffer.readableBytes();
  io_uring_prep_write(sqe, fd, buf, len, 0); // 从offset 0开始写入
  io_uring_sqe_set_data(sqe, reinterpret_cast<void *>(id));

  // 保存操作对象
  operations[id] = std::move(op);

  return true;
}

void IOUringHandler::poll(int timeout_ms) {
  // 提交所有准备好的操作
  int submitted = io_uring_submit(&ring);
  if (submitted < 0) {
    std::cerr << "io_uring_submit failed: " << strerror(-submitted)
              << std::endl;
    return;
  }

  // 处理完成事件
  struct io_uring_cqe *cqe;
  struct __kernel_timespec ts;
  if (timeout_ms >= 0) {
    ts.tv_sec = timeout_ms / 1000;
    ts.tv_nsec = (timeout_ms % 1000) * 1000000;
  }

  // 等待完成事件
  int ret = timeout_ms >= 0 ? io_uring_wait_cqes(&ring, &cqe, 1, &ts, NULL)
                            : io_uring_wait_cqe(&ring, &cqe);

  if (ret < 0 && ret != -ETIME) {
    std::cerr << "io_uring_wait_cqe failed: " << strerror(-ret) << std::endl;
    return;
  }

  // 处理所有可用的完成事件
  struct io_uring_cqe *cqes[128];
  int count = io_uring_peek_batch_cqe(&ring, cqes, 128);

  for (int i = 0; i < count; i++) {
    __u64 id = reinterpret_cast<__u64>(io_uring_cqe_get_data(cqes[i]));

    int res = cqes[i]->res;

    auto it = operations.find(id);
    if (it != operations.end()) {
      Operation *op = it->second.get();

      if (res < 0) {
        // 操作失败
        if (op->type == Operation::READ) {
          op->readCb(-1, -res);
        } else {
          op->writeCb(-1, -res);
        }
      } else {
        // 操作成功
        if (op->type == Operation::READ) {
          // 调整buffer的writerIndex
          op->buffer->retrieve(res);
          op->readCb(res, 0);
        } else {
          // 调整buffer的readerIndex
          op->buffer->retrieve(res);
          op->writeCb(res, 0);
        }
      }

      // 移除已完成的操作
      operations.erase(it);
    }

    // 标记事件为已处理
    io_uring_cqe_seen(&ring, cqes[i]);
  }
}