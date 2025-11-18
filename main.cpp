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

// size,sort_type,case,comparisons,copies
// 1000,insertion,sorted,499500,2000
// 1000,insertion,reverse,999,2000
// 1000,insertion,average,251543,2000
// 1000,comb,sorted,18713,2000
// 1000,comb,reverse,19712,2000
// 1000,comb,average,22469,2000
// 1000,quick,sorted,499500,1501497
// 1000,quick,reverse,499500,751497
// 1000,quick,average,10964,18746
// 2000,insertion,sorted,1999000,4000
// 2000,insertion,reverse,1999,4000
// 2000,insertion,average,1002051,4000
// 2000,comb,sorted,43383,4000
// 2000,comb,reverse,45382,4000
// 2000,comb,average,52558,4000
// 2000,quick,sorted,1999000,6002997
// 2000,quick,reverse,1999000,3002997
// 2000,quick,average,24814,41463
// 3000,insertion,sorted,4498500,6000
// 3000,insertion,reverse,2999,6000
// 3000,insertion,average,2256389,6000
// 3000,comb,sorted,68059,6000
// 3000,comb,reverse,71058,6000
// 3000,comb,average,82394,6000
// 3000,quick,sorted,4498500,13504497
// 3000,quick,reverse,4498500,6754497
// 3000,quick,average,39410,65835
// 4000,insertion,sorted,7998000,8000
// 4000,insertion,reverse,3999,8000
// 4000,insertion,average,4006631,8000
// 4000,comb,sorted,94726,8000
// 4000,comb,reverse,98725,8000
// 4000,comb,average,115880,8000
// 4000,quick,sorted,7998000,24005997
// 4000,quick,reverse,7998000,12005997
// 4000,quick,average,54688,89575
// 5000,insertion,sorted,12497500,10000
// 5000,insertion,reverse,4999,10000
// 5000,insertion,average,6252803,10000
// 5000,comb,sorted,123386,10000
// 5000,comb,reverse,128385,10000
// 5000,comb,average,147931,10000
// 5000,quick,sorted,12497500,37507497
// 5000,quick,reverse,12497500,18757497
// 5000,quick,average,70597,116085
// 6000,insertion,sorted,17997000,12000
// 6000,insertion,reverse,5999,12000
// 6000,insertion,average,9010410,12000
// 6000,comb,sorted,154056,12000
// 6000,comb,reverse,160055,12000
// 6000,comb,average,183631,12000
// 6000,quick,sorted,17997000,54008997
// 6000,quick,reverse,17997000,27008997
// 6000,quick,average,87679,141735
// 7000,insertion,sorted,24496500,14000
// 7000,insertion,reverse,6999,14000
// 7000,insertion,average,12261113,14000
// 7000,comb,sorted,186725,14000
// 7000,comb,reverse,193724,14000
// 7000,comb,average,207791,14000
// 7000,quick,sorted,24496500,73510497
// 7000,quick,reverse,24496500,36760497
// 7000,quick,average,104584,171600
// 8000,insertion,sorted,31996000,16000
// 8000,insertion,reverse,7999,16000
// 8000,insertion,average,16017938,16000
// 8000,comb,sorted,213390,16000
// 8000,comb,reverse,221389,16000
// 8000,comb,average,272982,16000
// 8000,quick,sorted,31996000,96011997
// 8000,quick,reverse,31996000,48011997
// 8000,quick,average,121221,198275
// 9000,insertion,sorted,40495500,18000
// 9000,insertion,reverse,8999,18000
// 9000,insertion,average,20259773,18000
// 9000,comb,sorted,249059,18000
// 9000,comb,reverse,258058,18000
// 9000,comb,average,276685,18000
// 9000,quick,sorted,40495500,121513497
// 9000,quick,reverse,40495500,60763497
// 9000,quick,average,137864,223681
// 10000,insertion,sorted,49995000,20000
// 10000,insertion,reverse,9999,20000
// 10000,insertion,average,25000262,20000
// 10000,comb,sorted,276739,20000
// 10000,comb,reverse,286738,20000
// 10000,comb,average,336832,20000
// 10000,quick,sorted,49995000,150014997
// 10000,quick,reverse,49995000,75014997
// 10000,quick,average,155564,251282
// 25000,insertion,sorted,312487500,50000
// 25000,insertion,reverse,24999,50000
// 25000,insertion,average,156172940,50000
// 25000,comb,sorted,766743,50000
// 25000,comb,reverse,791742,50000
// 25000,comb,average,953485,50000
// 25000,quick,sorted,312487500,937537497
// 25000,quick,reverse,312487500,468787497
// 25000,quick,average,434220,696389
// 50000,insertion,sorted,1249975000,100000
// 50000,insertion,reverse,49999,100000
// 50000,insertion,average,624877144,100000
// 50000,comb,sorted,1683412,100000
// 50000,comb,reverse,1733411,100000
// 50000,comb,average,2148902,100000
// 50000,quick,sorted,1249975000,3750074997
// 50000,quick,reverse,1249975000,1875074997
// 50000,quick,average,935135,1497076
// 100000,insertion,sorted,4999950000,200000
// 100000,insertion,reverse,99999,200000
// 100000,insertion,average,2500811209,200000
// 100000,comb,sorted,3666745,200000
// 100000,comb,reverse,3766744,200000
// 100000,comb,average,4029741,200000