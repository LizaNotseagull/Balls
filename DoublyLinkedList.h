#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include <cstddef>
#include <cstdlib>
#include <new>
#include "Node.h"
#include "exceptions.h"

/**
 * Двусвязный список с ручным управлением памятью.
 * Управление памятью: malloc/free + placement new + явный вызов деструктора.
 * Это удовлетворяет требованию "через конструктор и деструктор".
 */
template <typename T>
class DoublyLinkedList {
private:
    Node<T>* head;
    Node<T>* tail;
    size_t count;

public:
    DoublyLinkedList();
    ~DoublyLinkedList();
    DoublyLinkedList(const DoublyLinkedList&) = delete;
    DoublyLinkedList& operator=(const DoublyLinkedList&) = delete;

    // Основные операции со списком
    void push_back(const T& value);
    void push_front(const T& value);
    void insert_before(Node<T>* node, const T& value);
    void insert_after(Node<T>* node, const T& value);
    void remove(Node<T>* node);
    void remove_range(Node<T>* from, Node<T>* to);  // удаляет все узлы от from до to включительно
    void clear();

    Node<T>* get_head() const { return head; }
    Node<T>* get_tail() const { return tail; }
    size_t size() const { return count; }
    bool empty() const { return count == 0; }

    // Итераторы для удобного обхода (скрывают внутреннее устройство)
    class Iterator {
    private:
        Node<T>* ptr;
    public:
        Iterator(Node<T>* p = nullptr) : ptr(p) {}
        T& operator*() { return ptr->getData(); }
        const T& operator*() const { return ptr->getData(); }
        Iterator& operator++() { if (ptr) ptr = ptr->getNext(); return *this; }
        Iterator operator++(int) { Iterator tmp = *this; if (ptr) ptr = ptr->getNext(); return tmp; }
        bool operator==(const Iterator& other) const { return ptr == other.ptr; }
        bool operator!=(const Iterator& other) const { return ptr != other.ptr; }
        Node<T>* get_node() const { return ptr; }
    };

    class ConstIterator {
    private:
        const Node<T>* ptr;
    public:
        ConstIterator(const Node<T>* p = nullptr) : ptr(p) {}
        const T& operator*() const { return ptr->getData(); }
        ConstIterator& operator++() { if (ptr) ptr = ptr->getNext(); return *this; }
        ConstIterator operator++(int) { ConstIterator tmp = *this; if (ptr) ptr = ptr->getNext(); return tmp; }
        bool operator==(const ConstIterator& other) const { return ptr == other.ptr; }
        bool operator!=(const ConstIterator& other) const { return ptr != other.ptr; }
        const Node<T>* get_node() const { return ptr; }
    };

    Iterator begin() { return Iterator(head); }
    Iterator end() { return Iterator(nullptr); }
    ConstIterator begin() const { return ConstIterator(head); }
    ConstIterator end() const { return ConstIterator(nullptr); }
    ConstIterator cbegin() const { return ConstIterator(head); }
    ConstIterator cend() const { return ConstIterator(nullptr); }

private:
    // Выделение памяти под узел с вызовом конструктора через placement new
    Node<T>* createNode(const T& value);
    // Освобождение памяти и вызов деструктора
    void destroyNode(Node<T>* node);
};



template <typename T>
DoublyLinkedList<T>::DoublyLinkedList() : head(nullptr), tail(nullptr), count(0) {}

template <typename T>
DoublyLinkedList<T>::~DoublyLinkedList() { clear(); }

template <typename T>
void DoublyLinkedList<T>::push_back(const T& value) {
    Node<T>* newNode = createNode(value);
    if (!head) head = tail = newNode;
    else {
        tail->setNext(newNode);
        newNode->setPrev(tail);
        tail = newNode;
    }
    ++count;
}

template <typename T>
void DoublyLinkedList<T>::push_front(const T& value) {
    Node<T>* newNode = createNode(value);
    if (!head) head = tail = newNode;
    else {
        newNode->setNext(head);
        head->setPrev(newNode);
        head = newNode;
    }
    ++count;
}

template <typename T>
void DoublyLinkedList<T>::insert_before(Node<T>* node, const T& value) {
    if (!node) return;
    Node<T>* newNode = createNode(value);
    Node<T>* prevNode = node->getPrev();
    newNode->setPrev(prevNode);
    newNode->setNext(node);
    if (prevNode) prevNode->setNext(newNode);
    else head = newNode;
    node->setPrev(newNode);
    ++count;
}

template <typename T>
void DoublyLinkedList<T>::insert_after(Node<T>* node, const T& value) {
    if (!node) return;
    Node<T>* newNode = createNode(value);
    Node<T>* nextNode = node->getNext();
    newNode->setPrev(node);
    newNode->setNext(nextNode);
    if (nextNode) nextNode->setPrev(newNode);
    else tail = newNode;
    node->setNext(newNode);
    ++count;
}

template <typename T>
void DoublyLinkedList<T>::remove(Node<T>* node) {
    if (!node) return;
    Node<T>* prevNode = node->getPrev();
    Node<T>* nextNode = node->getNext();
    if (prevNode) prevNode->setNext(nextNode);
    else head = nextNode;
    if (nextNode) nextNode->setPrev(prevNode);
    else tail = prevNode;
    destroyNode(node);
    --count;
}

/**
 * Удаление диапазона узлов от from до to (включительно).
 * Сложность O(k), где k – количество удаляемых узлов.
 * После удаления соседние узлы (левый и правый) связываются напрямую.
 */
template <typename T>
void DoublyLinkedList<T>::remove_range(Node<T>* from, Node<T>* to) {
    if (!from || !to) return;
    // Запоминаем узлы, которые станут соседями после удаления
    Node<T>* left = from->getPrev();
    Node<T>* right = to->getNext();
    if (left) left->setNext(right);
    else head = right;
    if (right) right->setPrev(left);
    else tail = left;

    // Удаляем все узлы от from до to
    Node<T>* curr = from;
    while (curr != to) {
        Node<T>* nextNode = curr->getNext();
        destroyNode(curr);
        --count;
        curr = nextNode;
    }
    destroyNode(to);
    --count;
}

template <typename T>
void DoublyLinkedList<T>::clear() {
    Node<T>* curr = head;
    while (curr) {
        Node<T>* nextNode = curr->getNext();
        destroyNode(curr);
        curr = nextNode;
    }
    head = tail = nullptr;
    count = 0;
}

/**
 * Выделение памяти через malloc и вызов конструктора через placement new.
 * Это альтернатива оператору new, позволяющая явно управлять вызовом конструктора.
 */
template <typename T>
Node<T>* DoublyLinkedList<T>::createNode(const T& value) {
    void* mem = malloc(sizeof(Node<T>));
    if (!mem) throw std::bad_alloc();
    Node<T>* node = new(mem) Node<T>(value);  // placement new
    return node;
}

/**
 * Явный вызов деструктора и освобождение памяти через free.
 * Это пара к createNode.
 */
template <typename T>
void DoublyLinkedList<T>::destroyNode(Node<T>* node) {
    if (!node) return;
    node->~Node();   // явный вызов деструктора
    free(node);
}

#endif
