#ifndef NODE_H
#define NODE_H

/**
 * Узел двусвязного списка.
 * Шаблонный, позволяет хранить данные любого типа.
 * Инкапсуляция: поля приватные, доступ через геттеры/сеттеры.
 */
template <typename T>
class Node {
private:
    T data;          // хранимые данные
    Node* prev;      // указатель на предыдущий узел
    Node* next;      // указатель на следующий узел

public:
    Node(const T& value) : data(value), prev(nullptr), next(nullptr) {}

    // Геттеры (константные версии для чтения)
    T& getData() { return data; }
    const T& getData() const { return data; }
    Node* getPrev() const { return prev; }
    Node* getNext() const { return next; }

    // Сеттеры
    void setData(const T& value) { data = value; }
    void setPrev(Node* p) { prev = p; }
    void setNext(Node* n) { next = n; }
};

#endif