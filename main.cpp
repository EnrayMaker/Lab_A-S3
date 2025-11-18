#include <vector>
#include <cstddef> // size_t
#include <iostream>
#include <random>  // случайные числа

// Статистика
struct stats {
    size_t comparison_count = 0;
    size_t copy_count = 0;
};

// Дополнил до двусвязного списка
class DoublyLinkedList {
    struct Node {
        int data;      // Значение элемента
        Node* next;    // Указатель на следующий узел
        Node* prev;    // Указатель на предыдущий узел
        Node(int d) : data(d), next(nullptr), prev(nullptr) {} // Конструктор узла: инициализируем значение и указатели null
    };

    Node* head = nullptr; 
    Node* tail = nullptr; 

public:
    ~DoublyLinkedList() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }

    // Добавить элемент в конец списка
    void append(int d) {
        Node* newNode = new Node(d);
        if (tail) {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        } else {
            head = tail = newNode;
        }
    }

    // Заполнение списка из вектора
    void from_vector(const std::vector<int>& vec, stats& s) {
        for (int x : vec) {
            append(x);
            s.copy_count++; // добавить в статистику
        }
    }

    // из списка в вектор, с подсчетом копирований
    void to_vector(std::vector<int>& vec, stats& s) {
        vec.clear();
        Node* curr = head;
        while (curr) {
            vec.push_back(curr->data);
            s.copy_count++; // добавить в статистику
            curr = curr->next;
        }
    }

    // обмен двух узлов местами (перестановка указателей)
    void swap_nodes(Node* a, Node* b) {
        if (a == b) return;

        // узлы смежные
        if (a->next == b) {
            a->next = b->next;
            if (b->next) b->next->prev = a;
            b->prev = a->prev;
            if (a->prev) a->prev->next = b;
            else head = b;
            b->next = a;
            a->prev = b;
            if (tail == b) tail = a;
            return;
        }
        if (b->next == a) {
            swap_nodes(b, a);
            return;
        }

        // узлы не смежные
        Node* a_prev = a->prev;
        Node* a_next = a->next;
        Node* b_prev = b->prev;
        Node* b_next = b->next;

        if (a_prev) a_prev->next = b;
        else head = b;
        b->prev = a_prev;
        b->next = a_next;
        if (a_next) a_next->prev = b;

        if (b_prev) b_prev->next = a;
        else head = a;
        a->prev = b_prev;
        a->next = b_next;
        if (b_next) b_next->prev = a;

        if (tail == a) tail = b;
        else if (tail == b) tail = a;
    }

    // сортировка вставками на списке
    // перемещаем узлы, не копируя значения
    stats insertion_sort() {
        stats s;
        if (!head) return s;

        Node* sorted = nullptr; // указываем на голову отсортированного
        Node* curr = head;  // оригинальный список
        while (curr) {
            Node* next = curr->next;
            curr->prev = nullptr;
            curr->next = nullptr;

            Node* pos = sorted;  // элементы отсортированного
            Node* prev_pos = nullptr;  
            while (pos) { // движение по отсортированному
                s.comparison_count++;
                if (curr->data < pos->data) {
                    break;
                }
                prev_pos = pos;
                pos = pos->next;
            }

            if (!prev_pos) {
                curr->next = sorted;
                if (sorted) sorted->prev = curr;
                sorted = curr;
            } else {
                curr->next = pos;
                curr->prev = prev_pos;
                prev_pos->next = curr;
                if (pos) pos->prev = curr;
            }

            curr = next;
        }

        head = sorted;
        tail = sorted;
        while (tail && tail->next) tail = tail->next;
        return s;
    }

    // сортировка расчёской на списке (вектор указателей на узлы)
    stats comb_sort() {
        stats s;
        if (!head) return s;

        // вектор указателей на узлы для быстрого доступа
        std::vector<Node*> nodes;
        Node* temp = head;
        while (temp) {
            nodes.push_back(temp);
            temp = temp->next;
        }

        size_t n = nodes.size();
        size_t gap = n;
        bool swapped = true;

        while (gap > 1 || swapped) {
            gap = (gap * 10) / 13; // МГЕ паровозик, фактор сжатия 1.3 (10/13 ≈ 0.769)
            if (gap < 1) gap = 1;
            swapped = false;

            for (size_t i = 0; i + gap < n; ++i) {
                s.comparison_count++;
                if (nodes[i]->data > nodes[i + gap]->data) {
                    swap_nodes(nodes[i], nodes[i + gap]); // Мена? Мена!
                    std::swap(nodes[i], nodes[i + gap]); //  Мена? Мена!, но это вектор
                    swapped = true;
                }
            }
        }

        // на всякий случай)
        head = nodes[0];
        tail = nodes.back();
        return s;
    }
};

int main() {

    return 0;
}