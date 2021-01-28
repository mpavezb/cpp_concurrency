
#include <atomic>
#include <bits/c++config.h>

// =============================================================================
// Example: Lock-Free Queue - Uses atomic integer as exclusive index
// =============================================================================
// std::atomic is used as index to non-atomic memory.
// the fetch_add operation ensures any concurrent push() wont fall into the
// same index.
class atomic_queue {
  int queue[100];
  std::atomic<std::size_t> front;

  void push(int x) {
    std::size_t exclusive_index = front.fetch_add(1);
    queue[exclusive_index] = x;
  }
};

// =============================================================================
// Example: Lock Free List - Uses atomic pointer to non-atomic memory (head)
// =============================================================================
class atomic_list {
  struct node {
    int value;
    node *next;
  };

  std::atomic<node *> head;

  void push_front(int x) {
    node *new_n = new node;
    new_n->value = x;

    // if old_head == head, then do head = new_head;
    // otherwise, update old_head.
    node *old_head = head;
    do {
      // keep next reference updated.
      new_n->next = old_head;
    } while (!head.compare_exchange_strong(old_head, new_n));
  }
};

// =============================================================================
// Example:
// =============================================================================

int main() { return 0; }
