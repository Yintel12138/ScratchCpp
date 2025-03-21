// main.cpp
#include "Buffer.h"
#include "IOUringHandler.h"
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

// 递归式读取整个文件
void readEntireFile(IOUringHandler &handler, int fd, Buffer &buffer) {
  handler.readAsync(fd, buffer, [&](ssize_t bytes, int error) {
    if (error) {
      std::cerr << "Read error: " << strerror(error) << std::endl;
    } else if (bytes > 0) {
      // 更新buffer的写入位置，但不移动读取位置，以便累积数据
      buffer.retrieve(0); // 这里不移动读指针，只更新内部状态

      std::cout << "Read chunk of " << bytes << " bytes, buffer now has "
                << buffer.readableBytes() << " readable bytes" << std::endl;

      // 继续读取更多数据
      readEntireFile(handler, fd, buffer);
    } else {
      // bytes == 0 表示文件结束
      std::cout << "End of file reached, total bytes read: "
                << buffer.readableBytes() << std::endl;

      // 文件读取完成，可以处理整个文件的数据
      // 如果文件很大，不建议全部打印，这里只显示统计信息
    }
  });

  // 处理完成事件，等待读取操作完成
  handler.poll();
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
    return 1;
  }

  // 打开文件
  int fd = open(argv[1], O_RDONLY);
  if (fd < 0) {
    std::cerr << "Failed to open file: " << argv[1] << std::endl;
    return 1;
  }

  // 创建io_uring处理器
  IOUringHandler handler;

  // 创建Buffer
  Buffer readBuffer;

  // 读取整个文件
  std::cout << "Starting to read the entire file..." << std::endl;
  readEntireFile(handler, fd, readBuffer);

  // 文件读取完成后，可以处理整个缓冲区
  std::cout << "File reading completed" << std::endl;
  std::cout << "Total bytes in buffer: " << readBuffer.readableBytes()
            << std::endl;

  // 如果需要，可以将缓冲区内容转为字符串
  if (readBuffer.readableBytes() < 1024) {
    // 对于小文件，可以直接打印全部内容
    std::string fileContent = readBuffer.retrieveAllAsString();
    std::cout << "File content:\n" << fileContent << std::endl;
  } else {
    // 对于大文件，只打印开头部分
    std::string previewContent = readBuffer.retrieveAsString(1024);
    std::cout << "File preview (first 1KB):\n"
              << previewContent << "..." << std::endl;
  }

  // 后续代码 - 写入演示等...
  // 创建另一个Buffer进行写操作演示
  Buffer writeBuffer;
  const char *message = "Hello, io_uring with Buffer!";
  writeBuffer.append(message, strlen(message));

  // 打开输出文件
  int outfd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (outfd < 0) {
    std::cerr << "Failed to open output.txt" << std::endl;
    close(fd);
    return 1;
  }

  // 异步写入文件
  bool success =
      handler.writeAsync(outfd, writeBuffer, [](ssize_t bytes, int error) {
        if (error) {
          std::cerr << "Write error: " << strerror(error) << std::endl;
        } else {
          std::cout << "Successfully wrote " << bytes << " bytes" << std::endl;
        }
      });

  if (!success) {
    std::cerr << "Failed to submit write operation" << std::endl;
    close(fd);
    close(outfd);
    return 1;
  }

  // 处理完成事件
  handler.poll();

  // 关闭文件
  close(fd);
  close(outfd);

  return 0;
}