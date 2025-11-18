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

// сортировка вставками 
stats insertion_sort(std::vector<int>& vec) {
    stats s;
    DoublyLinkedList list;
    list.from_vector(vec, s);
    stats sort_stats = list.insertion_sort();
    s.comparison_count += sort_stats.comparison_count;
    s.copy_count += sort_stats.copy_count;
    list.to_vector(vec, s);
    return s;
}

// сортировка расчёской 
stats comb_sort(std::vector<int>& vec) {
    stats s;
    DoublyLinkedList list;
    list.from_vector(vec, s);
    stats sort_stats = list.comb_sort();
    s.comparison_count += sort_stats.comparison_count;
    s.copy_count += sort_stats.copy_count;
    list.to_vector(vec, s);
    return s;
}

// вспомогательная рекурсивная функция qs
void quick_sort_helper(std::vector<int>& vec, size_t low, size_t high, stats& s) {
    if (low >= high) return;

    int pivot = vec[high]; // последний для простоты
    size_t i = low;
    for (size_t j = low; j < high; ++j) {
        s.comparison_count++;
        if (vec[j] < pivot) {
            std::swap(vec[i], vec[j]);
            s.copy_count += 3;
            ++i;
        }
    }
    std::swap(vec[i], vec[high]);
    s.copy_count += 3;

    if (i > 0) quick_sort_helper(vec, low, i - 1, s);
    quick_sort_helper(vec, i + 1, high, s);
}

// основная функция быстрой сортировки
stats quick_sort(std::vector<int>& vec) {
    stats s;
    if (vec.empty()) return s;

    quick_sort_helper(vec, 0, vec.size() - 1, s);
    return s;
}

//функция выбора сортировки
stats choose_sort(const std::string& sort_type, std::vector<int>& data) {
    if (sort_type == "insertion") return insertion_sort(data);
    if (sort_type == "comb") return comb_sort(data);
    return quick_sort(data);
}

int main() {
    std::vector<size_t> sizes = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 25000, 50000, 100000};
    // типы сортировок
    std::vector<std::string> sort_types = {"insertion", "comb", "quick"};

    std::cout << "size,sort_type,case,comparisons,copies\n";

    for (size_t size : sizes) {
        for (const std::string& sort_type : sort_types) {
            // а) Отсортированный массив
            std::vector<int> data(size);
            for (size_t i = 0; i < size; ++i) data[i] = static_cast<int>(i);
            stats st = choose_sort(sort_type, data);
            std::cout << size << "," << sort_type << ",sorted," << st.comparison_count << "," << st.copy_count << "\n";

            // б) Обратно отсортированный массив
            for (size_t i = 0; i < size; ++i) data[i] = static_cast<int>(size - i);
            st = choose_sort(sort_type, data);
            std::cout << size << "," << sort_type << ",reverse," << st.comparison_count << "," << st.copy_count << "\n";

            // в) Среднее для 100 случайных массивов
            stats avg{0, 0};
            for (int run = 0; run < 100; ++run) {
                std::mt19937 gen(42 + run); // зерно
                std::uniform_int_distribution<int> dis(-1000000, 1000000);
                for (auto& x : data) x = dis(gen); // переиспользуем data для random
                stats current_st = choose_sort(sort_type, data);
                avg.comparison_count += current_st.comparison_count;
                avg.copy_count += current_st.copy_count;
            }
            avg.comparison_count /= 100;
            avg.copy_count /= 100;
            std::cout << size << "," << sort_type << ",average," << avg.comparison_count << "," << avg.copy_count << "\n";
        }
    }

    return 0;
}