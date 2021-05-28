#include<stdio.h>
#include<unistd.h>

#define offsetof(type, member) \
((size_t)(&((type *)0)->member))
#define to_struct(ptr, type, member) \
((type *)((char *)(ptr) - offsetof(type, member)))
#define le2page(le, member) \
to_struct((le), struct Page, member)

struct list_entry {
    struct list_entry *prev, *next;
};
typedef struct list_entry list_entry_t;

static inline void
list_init(list_entry_t *elm) {
    elm->prev = elm->next = elm;
}

static inline void
__list_add(list_entry_t *elm, list_entry_t *prev, list_entry_t *next) {
    prev->next = next->prev = elm;
    elm->next = next;
    elm->prev = prev;
}

static inline void
list_add_before(list_entry_t *listelm, list_entry_t *elm) {
    __list_add(elm, listelm->prev, listelm);
}

static inline void
list_add_after(list_entry_t *listelm, list_entry_t *elm) {
    __list_add(elm, listelm, listelm->next);
}

static inline void
__list_del(list_entry_t *prev, list_entry_t *next) {
    prev->next = next;
    next->prev = prev;
}

static inline void
list_del(list_entry_t *listelm) {
    __list_del(listelm->prev, listelm->next);
}

static inline list_entry_t *
list_next(list_entry_t *listelm) {
    return listelm->next;
}


/* 应用层 */
typedef struct {
    list_entry_t free_list;         // the list header of free pages
    int lock;
} free_area_t;
struct Page {
    int page;
    list_entry_t page_link;         // free list link
};

int main(int argc, char **argv) {
    free_area_t free_area;
    list_init(&(free_area.free_list));

    struct Page pages[4];
    struct Page *link;
    for (int i = 0; i < 4; i++) {
        pages[i].page = i;
        link = &pages[i];
        list_add_before(&free_area.free_list, &(link->page_link));
    }

    list_entry_t *le = &free_area.free_list;  //le是空闲块链表头指针
    while ((le = list_next(le)) != &free_area.free_list) { //从第一个节点开始遍历
        struct Page *p = le2page(le, page_link); //获取节点所在基于Page数据结构的变量
        printf("%d \n", p->page);
    }

    return 0;
}