#include <iostream>
#include <unordered_map>
class ListNode {
public:
  int _key;
  int _val;
  ListNode *next;
  ListNode *prev;
  ListNode(int key, int val)
      : _key(key), _val(val), next(nullptr), prev(nullptr) {}
};
class LRUCache {

private:
  // 双向链表
  ListNode *_dummy;
  // 容量
  int _capacity;
  // 当前大小
  int _size;
  // 哈希表
  std::unordered_map<int, ListNode *> _keyToNode;

  // 移动节点到头部
  void moveToHead(ListNode *node) {
    // 断开节点
    node->prev->next = node->next;
    node->next->prev = node->prev;
    // 插入到头部
    node->next = _dummy->next;
    node->prev = _dummy;
    _dummy->next = node;
    node->next->prev = node;
  }
  void addToHead(ListNode *node) {
    node->next = _dummy->next;
    node->prev = _dummy;
    _dummy->next = node;
    node->next->prev = node;
  }
  // 删除尾部节点
  void removeTail() {
    // 找到尾部节点
    ListNode *node = _dummy->prev;
    // 断开节点
    node->prev->next = node->next;
    node->next->prev = node->prev;
    // 删除哈希表中的节点
    _keyToNode.erase(node->_key);
    delete node;
    _size--;
  }

public:
  int get(int key) {
    if (_keyToNode.find(key) == _keyToNode.end()) {
      return -1;
    }
    // 从哈希表中找到节点
    ListNode *node = _keyToNode[key];
    // 移动节点到头部
    moveToHead(node);
    return node->_val;
  }
  bool put(int key, int value) {
    // 判断容量
    if (_size == _capacity) {
      removeTail();
    }
    // 如果存在 则moveToHead
    if (_keyToNode.find(key) != _keyToNode.end()) {
      ListNode *node = _keyToNode[key];
      node->_val = value;
      moveToHead(node);
    } else {
      // 创建一个节点
      ListNode *node = new ListNode(key, value);
      // 插入到头部
      addToHead(node);
      // 插入到哈希表
      _keyToNode[key] = node;
      _size++;
    }
    return true;
  }
  LRUCache(int capcity) : _capacity(capcity), _size(0) {
    _dummy = new ListNode(-1, -1);
    // 连接头尾
    _dummy->next = _dummy;
    _dummy->prev = _dummy;
  }
  ~LRUCache() {
    ListNode *cur = _dummy->next;
    while (cur != _dummy) {
      ListNode *node = cur;
      cur = cur->next;
      delete node;
    }
    delete _dummy;
    // 删除哈希表
    _keyToNode.clear();
  }
  // 配置条件编译
  // 调试用：打印缓存内容
  void printCache() {
    std::cout << "Cache (最近使用 -> 最少使用): ";
    ListNode *curr = _dummy->next;
    while (curr != _dummy) {
      std::cout << "(" << curr->_key << ":" << curr->_val << ") ";
      curr = curr->next;
    }
    std::cout << std::endl;
  }
};

// 在 LRUCache 类实现后添加

int main() {
  std::cout << "===== 测试 LRU Cache =====" << std::endl;

  // 测试 1: 基本功能测试
  std::cout << "\n测试 1: 基本功能测试" << std::endl;
  LRUCache cache1(2);

  cache1.put(1, 1);
  std::cout << "添加 (1,1)" << std::endl;
  cache1.printCache();

  cache1.put(2, 2);
  std::cout << "添加 (2,2)" << std::endl;
  cache1.printCache();

  std::cout << "获取 key=1: " << cache1.get(1) << std::endl; // 返回 1
  cache1.printCache();

  cache1.put(3, 3); // 该操作会使key=2的数据被淘汰
  std::cout << "添加 (3,3), 淘汰最久未使用的 key=2" << std::endl;
  cache1.printCache();

  std::cout << "获取 key=2: " << cache1.get(2) << std::endl; // 返回 -1 (未找到)

  cache1.put(4, 4); // 该操作会使key=1的数据被淘汰
  std::cout << "添加 (4,4), 淘汰最久未使用的 key=1" << std::endl;
  cache1.printCache();

  std::cout << "获取 key=1: " << cache1.get(1) << std::endl; // 返回 -1 (未找到)
  std::cout << "获取 key=3: " << cache1.get(3) << std::endl; // 返回 3
  std::cout << "获取 key=4: " << cache1.get(4) << std::endl; // 返回 4
  cache1.printCache();

  // 测试 2: 更新已存在的键
  std::cout << "\n测试 2: 更新已存在的键" << std::endl;
  LRUCache cache2(2);

  cache2.put(1, 1);
  cache2.put(2, 2);
  std::cout << "初始缓存: " << std::endl;
  cache2.printCache();

  cache2.put(1, 10); // 更新key=1的值
  std::cout << "更新 key=1 的值为 10:" << std::endl;
  cache2.printCache();

  cache2.put(3, 3); // 淘汰key=2
  std::cout << "添加 (3,3), 淘汰 key=2:" << std::endl;
  cache2.printCache();

  // 测试 3: 大小为1的缓存
  std::cout << "\n测试 3: 大小为1的缓存" << std::endl;
  LRUCache cache3(1);

  cache3.put(1, 1);
  std::cout << "添加 (1,1)" << std::endl;
  cache3.printCache();

  cache3.put(2, 2);
  std::cout << "添加 (2,2), 淘汰 key=1" << std::endl;
  cache3.printCache();

  std::cout << "获取 key=1: " << cache3.get(1) << std::endl; // 返回 -1
  std::cout << "获取 key=2: " << cache3.get(2) << std::endl; // 返回 2

  // 测试 4: 频繁访问同一个键
  std::cout << "\n测试 4: 频繁访问同一个键" << std::endl;
  LRUCache cache4(3);

  cache4.put(1, 1);
  cache4.put(2, 2);
  cache4.put(3, 3);
  std::cout << "初始缓存: " << std::endl;
  cache4.printCache();

  std::cout << "连续3次访问 key=1" << std::endl;
  cache4.get(1);
  cache4.get(1);
  cache4.get(1);
  cache4.printCache();

  cache4.put(4, 4); // 应该淘汰key=2
  std::cout << "添加 (4,4), 淘汰 key=2:" << std::endl;
  cache4.printCache();

  return 0;
}