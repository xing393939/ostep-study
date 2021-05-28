#include<stdio.h>
#include<unistd.h>

#define offset_of(type, member) \
((size_t)(&((type *)0)->member))
#define to_struct(ptr, type, member) \
((type *)((char *)(ptr) - offset_of(type, member)))
#define to_struct_by(le, struct_name, member) \
to_struct((le), struct struct_name, member)

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
    list_entry_t list_head;         // the list header of free pages
    int lock;
} free_area_table;
struct free_area_entry {
    int page;
    list_entry_t list_link;         // free list link
};
typedef struct {
    list_entry_t list_head;         // the list header of free pages
    int lock;
} buf_cache_table;
struct buf_cache_entry {
    int number;
    list_entry_t list_link;         // free list link
};

int main(int argc, char **argv) {
    /* free_area_table */
    buf_cache_table buf_cache;
    list_init(&buf_cache.list_head);
    struct buf_cache_entry buf[3];
    struct buf_cache_entry link0;
    for (int i = 0; i < 3; i++) {
        buf[i].number = i;
        list_add_after(&buf_cache.list_head, &buf[i].list_link);
    }
    list_entry_t *le0 = &buf_cache.list_head;
    while ((le0 = list_next(le0)) != &buf_cache.list_head) {
        struct buf_cache_entry * b = to_struct_by(le0, buf_cache_entry, list_link);
        printf("%d \n", b->number);
    }

    /* free_area_table */
    free_area_table free_area;
    list_init(&(free_area.list_head));
    struct free_area_entry pages[4];
    struct free_area_entry *link;
    for (int i = 0; i < 4; i++) {
        pages[i].page = i;
        link = &pages[i];
        list_add_before(&free_area.list_head, &(link->list_link));
    }
    list_entry_t *le = &free_area.list_head;  //le是空闲块链表头指针
    while ((le = list_next(le)) != &free_area.list_head) { //从第一个节点开始遍历
        struct free_area_entry *p = to_struct_by(le, free_area_entry, list_link); //获取节点所在基于Page数据结构的变量
        printf("%d \n", p->page);
    }

    return 0;
}