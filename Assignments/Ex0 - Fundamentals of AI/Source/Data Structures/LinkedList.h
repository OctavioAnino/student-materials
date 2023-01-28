#include <stdexcept>

// LinkedList class should go in the "ufl_cap4053::fundamentals" namespace!
namespace ufl_cap4053 { namespace fundamentals {
    template <class T> 
    class LinkedList {
        struct Node;
        public:
            class Iterator{
                friend class LinkedList;
                private:
                    Node *currentNode_;
                    Iterator(Node* node) : currentNode_(node) {};
                public:
                    T operator*() const {
                        return currentNode_->element_;
                    }
                    Iterator& operator++() {
                        currentNode_ = currentNode_->next_;
                        return *this;
                    }
                    bool operator==(Iterator const& rhs) {
                        return currentNode_ == rhs.currentNode_;
                    }
                    bool operator!=(Iterator const& rhs) {
                        return currentNode_ != rhs.currentNode_;
                    }
            };
            LinkedList<T>() {
                head_ = nullptr;
                tail_ = nullptr;
            };

            Iterator begin() const {
                return Iterator(head_);
            }

            Iterator end() const {
                return Iterator(nullptr);
            }

            bool isEmpty() const {
                return begin() == end();
            }

            T getFront() {
                if (isEmpty()) throw std::runtime_error("[ LinkedList<T>::getFront() ]: Empty list.");
                return head_->element_;
            }
            T getBack() {
                if (isEmpty()) throw std::runtime_error("[ LinkedList<T>::getBack() ]: Empty list.");
                return tail_->element_;
            }

            void enqueue(T element) {
                if (isEmpty()) {
                    head_ = new Node(element);
                    head_->prev_ = nullptr;
                    head_->next_ = nullptr;
                    tail_ = head_;
                }
                else {
                    tail_->next_ = new Node(element);
                    Node* enqueued = tail_->next_;
                    enqueued->next_ = nullptr;
                    enqueued->prev_ = tail_;
                    tail_ = enqueued;
                }
            }

            void dequeue() {
                if (!isEmpty()) {
                    if (tail_ == head_)
                    {
                        delete head_;
                        tail_ = nullptr;
                        head_ = nullptr;
                    }
                    else
                    {
                        Node* newHead = head_->next_;
                        delete head_;
                        head_ = newHead;
                        newHead->prev_ = nullptr;
                    }
                }
            }

            void pop() {
                if (!isEmpty())
                {
                    if (tail_ == head_) {
                        delete head_;
                        tail_ = nullptr;
                        head_ = nullptr;
                    }
                    else
                    {
                        Node* newTail = tail_->prev_;
                        delete tail_;
                        tail_ = newTail;
                        tail_->next_ = nullptr;
                    }
                }
            }

            void clear() {
                Node *curr = head_;
                while (curr) {
                    Node *to_delete = curr;
                    curr = curr->next_;
                    delete to_delete;
                }
                head_ = nullptr;
                tail_ = nullptr;
            }

            bool contains(T element) {
                Node* curr = head_;
                while (curr) {
                    if (curr->element_ == element) return true;
                    curr = curr->next_;
                }
                return false;
            }

            void remove(T element) {
                Node* curr = head_;
                while (curr) {
                    if (curr->element_ == element) {
                        Node* left = curr->prev_;
                        Node* right = curr->next_;
                        if (left == nullptr) {
                            head_ = head_->next_;
                            if(head_) head_->prev_ = nullptr;
                        }
                        else {
                            left->next_ = right;
                        }
                        if (right) right->prev_ = left;
                        delete curr;
                        return;
                    }
                    curr = curr->next_;
                }
            }
        private:
            struct Node {
                T element_;
                Node *next_;
                Node *prev_;
                Node(T element){
                    element_ = element;
                }
            };
            Node *head_;
            Node *tail_;

    };


} }  // namespace ufl_cap4053::fundamentals
